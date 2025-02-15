#include "xxhash64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XXHASH64_PRIME1 11400714785074694791ull
#define XXHASH64_PRIME2 14029467366897019727ull
#define XXHASH64_PRIME3 1609587929392839161ull
#define XXHASH64_PRIME4 9650029242287828579ull
#define XXHASH64_PRIME5 2870177450012600261ull

typedef struct Hash_XX64_State {
    UInt64   _total_length;
    UInt64   _vec[4];
    UInt64   _mem64[4];
    ByteSize _mem_size;
} Hash_XX64_State;

static Hash_XX64_State state;

void _hash_xx64_reset(void) {
    memset(&state, 0, sizeof(state));
    state._vec[0] = XXHASH64_PRIME1 + XXHASH64_PRIME2;
    state._vec[1] = XXHASH64_PRIME2 + XXHASH64_PRIME3;
    state._vec[2] = XXHASH64_PRIME3 + XXHASH64_PRIME4;
    state._vec[3] = XXHASH64_PRIME4 + XXHASH64_PRIME5;
}

VYTAL_INLINE UInt64 _hash_xx64_rotate(UInt64 value, UInt32 shift) {
    return (shift == 0) ? value : ((value << shift) | (value >> (64 - shift)));
}

VYTAL_INLINE UInt64 _hash_xx64_read(const VoidPtr ptr) {
    UInt64 value;
    memcpy(&value, ptr, sizeof(value));

#if IS_BIG_ENDIAN
    return BSWAP64(value);
#else
    return value;
#endif
}

VYTAL_INLINE UInt64 _hash_xx64_round(const UInt64 value, const UInt64 input) {
    return (_hash_xx64_rotate(value + (input * XXHASH64_PRIME2), 31) * XXHASH64_PRIME1);
}

VYTAL_INLINE UInt64 _hash_xx64_avalance(const UInt64 hash) {
    UInt64 hash_ = hash;

    hash_ ^= hash_ >> 33;
    hash_ *= XXHASH64_PRIME2;
    hash_ ^= hash_ >> 29;
    hash_ *= XXHASH64_PRIME3;
    hash_ ^= hash_ >> 32;

    return hash_;
}

#if defined(__AVX2__)

#    include <immintrin.h>

void _hash_xx64_update(const VoidPtr input, const ByteSize input_length) {
    BytePtr pbegin_ = (BytePtr)input;
    BytePtr pend_   = pbegin_ + input_length;

    state._total_length += input_length;

    if ((state._mem_size + input_length) < 32) {
        // fill in the temp buffer
        BytePtr pfill_ = (BytePtr)state._mem64 + state._mem_size;
        memcpy(pfill_, input, input_length);

        state._mem_size += input_length;
        return;
    }

    if (state._mem_size > 0) {
        BytePtr  pfill_     = (BytePtr)state._mem64 + state._mem_size;
        ByteSize fill_size_ = 32 - state._mem_size;

        // fill the remainder of the buffer
        memcpy(pfill_, input, fill_size_);

        pbegin_ += fill_size_;
        state._mem_size = 0;
    }

    if ((pbegin_ + 32) <= pend_) {
        __m256i v1_ = _mm256_set_epi64x(state._vec[0], state._vec[1], state._vec[2], state._vec[3]);

        do {
            __m256i data_ = _mm256_loadu_si256((const __m256i *)pbegin_);
            __m256i mult_ = _mm256_set1_epi64x(XXHASH64_PRIME2);
            __m256i acc_  = _mm256_mul_epu32(data_, mult_);
            acc_          = _mm256_or_si256(_mm256_slli_epi64(acc_, 31), _mm256_srli_epi64(acc_, 33));
            acc_          = _mm256_mul_epu32(acc_, _mm256_set1_epi64x(XXHASH64_PRIME1));
            v1_           = _mm256_add_epi64(v1_, acc_);

            pbegin_ += 32;
        } while ((pbegin_ + 32) <= pend_);

        _mm256_storeu_si256((__m256i *)state._vec, v1_);
    }

    if (pbegin_ < pend_) {
        memcpy(state._mem64, pbegin_, (ByteSize)(pend_ - pbegin_));
        state._mem_size = (ByteSize)(pend_ - pbegin_);
    }
}

#else

void _hash_xx64_update(const VoidPtr input, const ByteSize input_length) {
    BytePtr pbegin_ = (BytePtr)input;
    BytePtr pend_   = pbegin_ + input_length;

    state._total_length += input_length;

    if ((state._mem_size + input_length) < 32) {
        // fill in the temp buffer
        BytePtr pfill_ = (BytePtr)state._mem64 + state._mem_size;
        memcpy(pfill_, input, input_length);

        state._mem_size += input_length;
        return;
    }

    if (state._mem_size > 0) {
        BytePtr  pfill_     = (BytePtr)state._mem64 + state._mem_size;
        ByteSize fill_size_ = 32 - state._mem_size;

        // fill the remainder of the buffer
        memcpy(pfill_, input, fill_size_);

        // process the remainder
        {
            state._vec[0] = _hash_xx64_round(state._vec[0], _hash_xx64_read(state._mem64 + 0));
            state._vec[1] = _hash_xx64_round(state._vec[1], _hash_xx64_read(state._mem64 + 1));
            state._vec[2] = _hash_xx64_round(state._vec[2], _hash_xx64_read(state._mem64 + 2));
            state._vec[3] = _hash_xx64_round(state._vec[3], _hash_xx64_read(state._mem64 + 3));
        }

        pbegin_ += fill_size_;
        state._mem_size = 0;
    }

    if ((pbegin_ + 32) <= pend_) {
        BytePtr limit_ = pend_ - 32;

        do {
            state._vec[0] = _hash_xx64_round(state._vec[0], _hash_xx64_read(pbegin_ + 0));
            state._vec[1] = _hash_xx64_round(state._vec[1], _hash_xx64_read(pbegin_ + 8));
            state._vec[2] = _hash_xx64_round(state._vec[2], _hash_xx64_read(pbegin_ + 16));
            state._vec[3] = _hash_xx64_round(state._vec[3], _hash_xx64_read(pbegin_ + 24));
            pbegin_ += 32;
        } while (pbegin_ <= limit_);
    }

    if (pbegin_ < pend_) {
        memcpy(state._mem64, pbegin_, (ByteSize)(pend_ - pbegin_));
        state._mem_size = (UInt32)(pend_ - pbegin_);
    }
}

#endif

UInt64 _hash_xx64_finalize(void) {
    UInt64  hash_;
    BytePtr pbegin_ = (BytePtr)state._mem64;
    BytePtr pend_   = pbegin_ + state._mem_size;

    if (state._total_length > 32)
        hash_ = _hash_xx64_rotate(state._vec[0], 1) + _hash_xx64_rotate(state._vec[1], 7) + _hash_xx64_rotate(state._vec[2], 12) + _hash_xx64_rotate(state._vec[3], 18);
    else
        hash_ = state._vec[2] + XXHASH64_PRIME5;

    hash_ += state._total_length;

    while ((pbegin_ + 8) <= pend_) {
        UInt64 k1_ = _hash_xx64_read(pbegin_);
        k1_ *= XXHASH64_PRIME2;
        k1_ = _hash_xx64_rotate(k1_, 31);
        k1_ *= XXHASH64_PRIME1;

        hash_ ^= k1_;
        hash_ = (_hash_xx64_rotate(hash_, 27) * XXHASH64_PRIME1) + XXHASH64_PRIME4;

        pbegin_ += 8;
    }

    if ((pbegin_ + 4) <= pend_) {
        hash_ ^= *((UInt64 *)pbegin_) * XXHASH64_PRIME1;
        hash_ = (_hash_xx64_rotate(hash_, 23) * XXHASH64_PRIME2) + XXHASH64_PRIME3;

        pbegin_ += 4;
    }

    while (pbegin_ < pend_) {
        hash_ ^= (*pbegin_) * XXHASH64_PRIME5;
        hash_ = _hash_xx64_rotate(hash_, 11) * XXHASH64_PRIME1;

        pbegin_++;
    }

    return _hash_xx64_avalance(hash_);
}

HashedInt hash_xx64_buffer(const VoidPtr buffer, const ByteSize size) {
    if (!buffer || (size == 0))
        return 0;

    _hash_xx64_reset();

    BytePtr pbegin_ = (BytePtr)buffer;
    BytePtr pend_   = pbegin_ + size;

    while ((pbegin_ + 32) <= pend_) {
        _hash_xx64_update(pbegin_, 32);
        pbegin_ += 32;
    }

    if (pbegin_ < pend_)
        _hash_xx64_update(pbegin_, (ByteSize)(pend_ - pbegin_));

    return _hash_xx64_finalize();
}