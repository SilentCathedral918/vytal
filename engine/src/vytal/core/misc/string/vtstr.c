#include "vtstr.h"

#include <emmintrin.h>
#include <stdio.h>

// integrate vectorization
#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
#    define load_m128i(ptr) _mm_load_si128(VT_CAST(__m128i const *, ptr))
#    define loadu_m128i(ptr) _mm_load_si128(VT_CAST(__m128i const *, ptr))
#    define storeu_m128i(ptr, vec) _mm_storeu_si128(VT_CAST(__m128i_u *, ptr), vec)
#    define compare_m128i(left, right) _mm_movemask_epi8(_mm_cmpeq_epi8((left), (right)))
#    define load16(ptr) (*VT_CAST(const UInt16 *, ptr))
#    define load32(ptr) (*VT_CAST(const UInt32 *, ptr))
#    define find_first_set(value) (__builtin_ffs(value))
#    define REGISTER_SIZE 16

Str _misc_str_strstr_sse2_lowercase(Str target) {
    ByteSize target_length_ = strlen(target);
    __m128i  lower_mask_    = _mm_set1_epi8(0x20);
    UIntPtr  ptarget_head_  = VT_CAST(UIntPtr, target);

    while (target_length_ >= REGISTER_SIZE) {
        __m128i chunk_ = loadu_m128i(target);
        __m128i res_   = _mm_or_si128(chunk_, lower_mask_);
        storeu_m128i(target, res_);

        target += REGISTER_SIZE;
        target_length_ -= REGISTER_SIZE;
    }

    if (target_length_ > 0) {
        __m128i chunk_ = _mm_setzero_si128();
        memcpy(&chunk_, target, target_length_);

        __m128i res_ = _mm_or_si128(chunk_, lower_mask_);
        memcpy(target, &res_, target_length_);
    }

    return VT_CAST(Str, ptarget_head_);
}

ConstStr _misc_str_strstr_sse2_scanstr(Str str, const Char pattern[2]) {
    __m128i vec_zero_     = _mm_setzero_si128();
    __m128i vec_pat0_     = _mm_set1_epi8(pattern[0]);
    __m128i vec_pat1_     = _mm_set1_epi8(pattern[1]);
    UInt32  align_offset_ = (REGISTER_SIZE - 1) & VT_CAST(IntPtr, str);
    UInt16  pattern_pair_ = load16(pattern);

    if (align_offset_ > 0) {
        __m128i  data_  = load_m128i(str - align_offset_);
        unsigned zero_  = compare_m128i(vec_zero_, data_) >> align_offset_;
        unsigned match_ = ((compare_m128i(vec_pat0_, data_) & (compare_m128i(vec_pat1_, data_) >> 1)) >> align_offset_) &
                          ~zero_ & (zero_ - 1);

        if (zero_)
            return NULL;
        if (match_)
            return VT_CAST(ConstStr, str + find_first_set(match_) - 1);

        str += REGISTER_SIZE - align_offset_;
        if (load16(str - 1) == pattern_pair_)
            return VT_CAST(ConstStr, str - 1);
    }

    while (str > 0) {
        __m128i  data_  = load_m128i(str - align_offset_);
        unsigned zero_  = compare_m128i(vec_zero_, data_);
        unsigned match_ = compare_m128i(vec_pat0_, data_) & (compare_m128i(vec_pat1_, data_) >> 1) & ~zero_ & (zero_ - 1);

        if (zero_)
            return NULL;
        if (match_)
            return VT_CAST(ConstStr, str + find_first_set(match_) - 1);

        str += REGISTER_SIZE;
        if (load16(str - 1) == pattern_pair_)
            return VT_CAST(ConstStr, str - 1);
    }

    return NULL;
}

ConstStr _misc_str_strstr_sse2(Str str, ConstStr substr, Bool sensitive) {
    ByteSize str_length_    = strlen(str);
    ByteSize substr_length_ = strlen(substr);

    if (substr_length_ == 0 || str_length_ < substr_length_)
        return NULL;

    if (!sensitive) {
        Str     str_        = strdup(str);
        UIntPtr pnstr_head_ = VT_CAST(UIntPtr, str_);
        Str     substr_     = strdup(substr);

        _misc_str_strstr_sse2_lowercase(str_);
        _misc_str_strstr_sse2_lowercase(substr_);

        for (; (str_ = VT_CAST(Str, _misc_str_strstr_sse2_scanstr(str_, substr_))); ++str_) {
            if (!memcmp(str_ + 2, substr_ + 2, substr_length_ - 2)) {
                free(substr_);
                return VT_CAST(ConstStr, str + (VT_CAST(UIntPtr, str_) - pnstr_head_));
            }
        }

        free(str_);
        free(substr_);
    } else {
        for (; (str = VT_CAST(Str, _misc_str_strstr_sse2_scanstr(str, substr))); ++str) {
            if (!memcmp(str + 2, substr + 2, substr_length_ - 2))
                return VT_CAST(ConstStr, str);
        }
    }

    return NULL;
}
#endif

Str _misc_str_strstr_blk_lowercase(Str target) {
    ByteSize target_length_ = strlen(target);
    ByteSize idx_           = 0;
    UInt8    lower_mask_8_  = 0x20;
    UInt64   lower_mask_64_ = 0x2020202020202020ull;

// int128 is supported
#if defined(__SIZEOF_INT128__)
    for (; idx_ + 15 < target_length_; idx_ += 16) {
        Int128 *chunk_ = VT_CAST(Int128 *, target + idx_);

        // apply lowercase-mask for conversion
        *chunk_ |= VT_CAST(Int128, lower_mask_64_) << 64 | lower_mask_64_;
    }

    // handle the remaining bytes
    for (; idx_ < target_length_; ++idx_)
        target[idx_] |= lower_mask_8_;

    return target;

// int128 not supported
// > process two uint64 chunks
#else
    for (; idx_ + 15 < target_length_; idx_ += 16) {
        UInt64 *chunk1_ = VT_CAST(UInt64 *, target + idx_);
        UInt64 *chunk2_ = VT_CAST(UInt64 *, target + idx_ + 8);

        // apply lowercase-mask for conversion
        *chunk1_ |= lower_mask_64_;
        *chunk2_ |= lower_mask_64_;
    }

    // handle the remaining bytes
    for (; idx_ < target_length_; ++idx_)
        target[idx_] |= lower_mask_8_;

    return target;

#endif
}

// fallback to standard strstr() in case vectorization is not viable
ConstStr _misc_str_strstr_fallback(Str str, ConstStr substr, Bool sensitive) {
    Str str_ = NULL, substr_ = NULL;

    if (!sensitive) {
        str_    = strdup(str);
        substr_ = strdup(substr);
        _misc_str_strstr_blk_lowercase(str_);
        _misc_str_strstr_blk_lowercase(substr_);

        Str      res_    = strstr(str_, substr_);
        ByteSize offset_ = res_ - str_;

        free(str_);
        free(substr_);

        return (str + offset_);
    } else {
        return strstr(str, substr);
    }
}

ConstStr misc_str_strstr(Str str, ConstStr substr, Bool sensitive) {
    if (!str || !substr)
        return NULL;

#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
    return _misc_str_strstr_sse2(str, substr, sensitive);
#else
    return _misc_str_strstr_fallback(str, substr, sensitive);
#endif
}
