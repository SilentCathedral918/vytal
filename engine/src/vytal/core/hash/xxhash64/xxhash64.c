#include "xxhash64.h"

#include "vytal/core/hal/memory/vtmem.h"

#include <emmintrin.h>

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

void             _hash_xx64_reset(void) { hal_mem_memzero(&state, sizeof(Hash_XX64_State)); }
VT_INLINE UInt64 _hash_xx64_rotate(UInt64 value, UInt32 shift) { return (shift == 0) ? value : ((value << shift) | (value >> (64 - shift))); }
VT_INLINE UInt64 _hash_xx64_read(const VoidPtr ptr, const Bool big_endian) {
    return big_endian ? (VT_CAST(BytePtr, ptr)[7] | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[6]) << 8) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[5]) << 16) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[4]) << 24) |
                         (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[3]) << 32) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[2]) << 40) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[1]) << 48) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[0]) << 56))
                      : (VT_CAST(BytePtr, ptr)[0] | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[1]) << 8) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[2]) << 16) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[3]) << 24) |
                         (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[4]) << 32) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[5]) << 40) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[6]) << 48) | (VT_CAST(UInt64, VT_CAST(BytePtr, ptr)[7]) << 56));
}
VT_INLINE UInt64 _hash_xx64_round(const UInt64 value, const UInt64 input) { return (_hash_xx64_rotate(value + (input * XXHASH64_PRIME2), 31) * XXHASH64_PRIME1); }
UInt64           _hash_xx64_avalance(const UInt64 hash) {
    UInt64 hash_ = hash;

    hash_ ^= hash_ >> 33;
    hash_ *= XXHASH64_PRIME2;
    hash_ ^= hash_ >> 29;
    hash_ *= XXHASH64_PRIME3;
    hash_ ^= hash_ >> 32;

    return hash_;
}
Bool _hash_xx64_update(const VoidPtr input, const ByteSize input_length) {
    BytePtr pbegin_ = VT_CAST(BytePtr, input);
    BytePtr pend_   = pbegin_ + input_length;

    state._total_length += input_length;

    if ((state._mem_size + input_length) < 32) {
        // fill in the temp buffer
        BytePtr pfill_ = VT_CAST(BytePtr, state._mem64) + state._mem_size;
        hal_mem_memcpy(pfill_, input, input_length);
        state._mem_size += VT_CAST(UInt32, input_length);
        return false;
    }

#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
    if (state._mem_size > 0) {
        BytePtr  pfill_     = VT_CAST(BytePtr, state._mem64) + state._mem_size;
        ByteSize fill_size_ = VT_CAST(ByteSize, 32 - state._mem_size);

        // fill the remainder of the buffer
        hal_mem_memcpy(pfill_, input, fill_size_);

        __m128i chunk1_ = _mm_loadu_si128(VT_CAST(__m128i *, state._mem64));
        __m128i chunk2_ = _mm_loadu_si128(VT_CAST(__m128i *, state._mem64 + 2));

        // first pair of 64-bit blocks
        {
            chunk1_ = _mm_add_epi64(chunk1_, _mm_set1_epi64x(XXHASH64_PRIME2));
            chunk1_ = _mm_xor_si128(_mm_slli_epi64(chunk1_, 31), _mm_srli_epi64(chunk1_, 33));
            chunk1_ = _mm_mul_epu32(chunk1_, _mm_set1_epi64x(XXHASH64_PRIME1));
        }

        // second pair of 64-bit blocks
        {
            chunk2_ = _mm_add_epi64(chunk2_, _mm_set1_epi64x(XXHASH64_PRIME2));
            chunk2_ = _mm_xor_si128(_mm_slli_epi64(chunk2_, 31), _mm_srli_epi64(chunk2_, 33));
            chunk2_ = _mm_mul_epu32(chunk2_, _mm_set1_epi64x(XXHASH64_PRIME1));
        }

        // save results
        {
            _mm_storeu_si128(VT_CAST(__m128i *, state._vec), chunk1_);
            _mm_storeu_si128(VT_CAST(__m128i *, state._vec + 2), chunk2_);
        }

        pbegin_ += fill_size_;
        state._mem_size = 0;
    }

    while (pbegin_ + 32 <= pend_) {
        __m128i chunk1_ = _mm_loadu_si128(VT_CAST(__m128i *, pbegin_));
        __m128i chunk2_ = _mm_loadu_si128(VT_CAST(__m128i *, pbegin_ + 16));

        // process first chunk
        {
            chunk1_ = _mm_add_epi64(chunk1_, _mm_set1_epi64x(XXHASH64_PRIME2));
            chunk1_ = _mm_xor_si128(_mm_slli_epi64(chunk1_, 31), _mm_srli_epi64(chunk1_, 33));
            chunk1_ = _mm_mul_epu32(chunk1_, _mm_set1_epi64x(XXHASH64_PRIME1));
        }

        // process second chunk
        {
            chunk2_ = _mm_add_epi64(chunk2_, _mm_set1_epi64x(XXHASH64_PRIME2));
            chunk2_ = _mm_xor_si128(_mm_slli_epi64(chunk2_, 31), _mm_srli_epi64(chunk2_, 33));
            chunk2_ = _mm_mul_epu32(chunk2_, _mm_set1_epi64x(XXHASH64_PRIME1));
        }

        // accumulate results and pass into state
        {
            __m128i state1_ = _mm_loadu_si128(VT_CAST(__m128i *, state._vec));
            __m128i state2_ = _mm_loadu_si128(VT_CAST(__m128i *, state._vec + 2));

            state1_ = _mm_add_epi64(state1_, chunk1_);
            state2_ = _mm_add_epi64(state2_, chunk2_);

            _mm_storeu_si128(VT_CAST(__m128i *, state._vec), state1_);
            _mm_storeu_si128(VT_CAST(__m128i *, state._vec + 2), state2_);
        }

        pbegin_ += 32;
    }

    // handle remaining bytes
    if (pbegin_ < pend_) {
        hal_mem_memcpy(state._mem64, pbegin_, VT_CAST(ByteSize, pend_ - pbegin_));
        state._mem_size = VT_CAST(UInt32, pend_ - pbegin_);
    }

#else
    if (state._mem_size > 0) {
        BytePtr  pfill_     = VT_CAST(BytePtr, state._mem64) + state._mem_size;
        ByteSize fill_size_ = VT_CAST(ByteSize, 32 - state._mem_size);

        // fill the remainder of the buffer
        hal_mem_memcpy(pfill_, input, fill_size_);

        // process the remainder
        {
            state._vec[0] = _hash_xx64_round(state._vec[0], _hash_xx64_read(state._mem64 + 0, false));
            state._vec[1] = _hash_xx64_round(state._vec[1], _hash_xx64_read(state._mem64 + 1, false));
            state._vec[2] = _hash_xx64_round(state._vec[2], _hash_xx64_read(state._mem64 + 2, false));
            state._vec[3] = _hash_xx64_round(state._vec[3], _hash_xx64_read(state._mem64 + 3, false));
        }

        pbegin_ += fill_size_;
        state._mem_size = 0;
    }

    if ((pbegin_ + 32) <= pend_) {
        BytePtr limit_ = pend_ - 32;

        do {
            state._vec[0] = _hash_xx64_round(state._vec[0], _hash_xx64_read(pbegin_ + 0, false));
            state._vec[1] = _hash_xx64_round(state._vec[1], _hash_xx64_read(pbegin_ + 8, false));
            state._vec[2] = _hash_xx64_round(state._vec[2], _hash_xx64_read(pbegin_ + 16, false));
            state._vec[3] = _hash_xx64_round(state._vec[3], _hash_xx64_read(pbegin_ + 24, false));
            pbegin_ += 32;
        } while (pbegin_ <= limit_);
    }

    if (pbegin_ < pend_) {
        hal_mem_memcpy(state._mem64, pbegin_, VT_CAST(ByteSize, VT_CAST(PtrDiff, pend_ - pbegin_)));
        state._mem_size = VT_CAST(UInt32, pend_ - pbegin_);
    }

#endif

    return true;
}
UInt64 _hash_xx64_finalize(void) {
    UInt64  hash_;
    BytePtr pbegin_ = VT_CAST(BytePtr, state._mem64);
    BytePtr pend_   = pbegin_ + state._mem_size;

    if (state._total_length >= 32)
        hash_ = _hash_xx64_rotate(state._vec[0], 1) + _hash_xx64_rotate(state._vec[1], 7) + _hash_xx64_rotate(state._vec[2], 12) + _hash_xx64_rotate(state._vec[3], 18);
    else
        hash_ = state._vec[2] + XXHASH64_PRIME5;

    hash_ += state._total_length;

    while ((pbegin_ + 8) <= pend_) {
        UInt64 k1_ = _hash_xx64_read(pbegin_, false);
        k1_ *= XXHASH64_PRIME2;
        k1_ = _hash_xx64_rotate(k1_, 31);
        k1_ *= XXHASH64_PRIME1;

        hash_ ^= k1_;
        hash_ = (_hash_xx64_rotate(hash_, 27) * XXHASH64_PRIME1) + XXHASH64_PRIME4;

        pbegin_ += 8;
    }

    if ((pbegin_ + 4) <= pend_) {
        hash_ ^= VT_CAST(UInt64, *(VT_CAST(UInt32 *, pbegin_))) * XXHASH64_PRIME1;
        hash_ = (_hash_xx64_rotate(hash_, 23) * XXHASH64_PRIME2) + XXHASH64_PRIME3;

        pbegin_ += 4;
    }

    while (pbegin_ < pend_) {
        hash_ ^= VT_CAST(UInt64, *pbegin_) * XXHASH64_PRIME5;
        hash_ = _hash_xx64_rotate(hash_, 11) * XXHASH64_PRIME1;

        pbegin_++;
    }

    return _hash_xx64_avalance(hash_);
}

HashedInt hash_xx64_hashbuffer(const VoidPtr buffer, const ByteSize size) {
    if (!buffer || (size == 0))
        return 0;

    _hash_xx64_reset();

    BytePtr pbegin_ = VT_CAST(BytePtr, buffer);
    BytePtr pend_   = pbegin_ + size;

    while ((pbegin_ + 32) <= pend_) {
        _hash_xx64_update(pbegin_, 32);
        pbegin_ += 32;
    }

    if (pbegin_ < pend_)
        _hash_xx64_update(pbegin_, VT_CAST(PtrDiff, pend_ - pbegin_));

    return _hash_xx64_finalize();
}
