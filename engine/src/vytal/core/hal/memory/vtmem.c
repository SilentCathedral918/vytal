#include "vtmem.h"

#include <emmintrin.h>

void hal_mem_memswap(VoidPtr left, VoidPtr right, ByteSize size) {
#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
    BytePtr pleft_  = VT_CAST(BytePtr, left);
    BytePtr pright_ = VT_CAST(BytePtr, right);

    // handle chunks of 16 bytes
    while (size >= 16) {
        __m128i vec_left_  = _mm_load_si128(VT_CAST(__m128i *, pleft_));
        __m128i vec_right_ = _mm_load_si128(VT_CAST(__m128i *, pright_));

        _mm_store_si128(VT_CAST(__m128i *, pleft_), vec_right_);
        _mm_store_si128(VT_CAST(__m128i *, pright_), vec_left_);

        pleft_ += 16;
        pright_ += 16;
        size -= 16;
    }

    // handle remaining bytes
    for (ByteSize i = 0; i < size; i++) {
        Char temp_ = pleft_[i];
        pleft_[i]  = pright_[i];
        pright_[i] = temp_;
    }

#else
    const ByteSize threshold = 65536; // 64 KB

    if (size <= threshold) {
        SizePtr pleft_    = left;
        SizePtr pright_   = right;
        SizePtr pleftend_ = VT_CAST(SizePtr, left) + (size / sizeof(ByteSize));

        // buffer chunks of ByteSize bytes
        while (pleft_ < pleftend_) {
            ByteSize temp_ = *pleft_;
            *pleft_        = *pright_;
            *pright_       = temp_;
            ++pleft_;
            ++pright_;
        }

        // remaining bytes
        UBytePtr pleftrem_  = VT_CAST(UBytePtr, pleft_);
        UBytePtr prightrem_ = VT_CAST(UBytePtr, pright_);
        for (ByteSize i = 0; i < size % sizeof(ByteSize); ++i) {
            UInt8 temp_ = *pleftrem_;
            *pleftrem_  = *prightrem_;
            *prightrem_ = temp_;

            ++pleftrem_;
            ++prightrem_;
        }
    } else {
        VoidPtr temp_ = hal_mem_malloc(size);

        hal_mem_memcpy(temp_, left, size);
        hal_mem_memcpy(left, right, size);
        hal_mem_memcpy(right, temp_, size);

        hal_mem_free(temp_);
    }

#endif
}
