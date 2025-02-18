#include "string.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined(__AVX2__) || defined(__SSE__)
#    include <immintrin.h>
#endif

#if defined(_MSC_VER)
#    include <windows.h>
#endif

#include "vytal/core/memory/zone/memory_zone.h"

struct Container_String {
    Str      _data;
    ByteSize _size;
    ByteSize _capacity;
};

VYTAL_INLINE ByteSize _container_string_apply_alignment(const ByteSize size, const ByteSize alignment) {
    return ((size + (alignment - 1)) / alignment) * alignment;
}

VYTAL_INLINE ContainerResult _container_string_resize(String *str, const ByteSize new_capacity) {
    ByteSize new_alloc_size_ = sizeof(struct Container_String) + new_capacity;

    String old_str_ = *str;
    String new_str_ = NULL;
    if (memory_zone_allocate("Strings", new_alloc_size_, (VoidPtr *)&new_str_) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    new_str_->_size     = (*str)->_size;
    new_str_->_capacity = new_capacity;
    new_str_->_data     = (Str)((UIntPtr)new_str_ + sizeof(struct Container_String));

    memcpy(new_str_->_data, (*str)->_data, (*str)->_size);

    if (memory_zone_deallocate("Strings", old_str_, old_str_->_capacity) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    *str = new_str_;
    return CONTAINER_SUCCESS;
}

VYTAL_INLINE Int32 _container_string_ctz(UInt32 mask) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz(mask);

#elif defined(_MSC_VER)
    UInt32 index_;
    _BitScanForward(&index_, mask);
    return (Int32)index_;

#else
    if (mask == 0) return 32;

    Int32 count_ = 0;
    while ((mask & 1) == 0) {
        mask >>= 1;
        ++count_;
    }
    return count_;

#endif
}

ContainerResult container_string_construct(ConstStr content, String *out_new_str) {
    if (!content) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize content_length_ = strlen(content);
    // multiply by CONTAINER_RESIZE_FACTOR to prevent frequent re-allocations early on
    ByteSize capacity_   = _container_string_apply_alignment(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
    ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("Strings", alloc_size_, (VoidPtr *)out_new_str) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_size     = content_length_;
    (*out_new_str)->_capacity = capacity_;
    (*out_new_str)->_data     = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

    memcpy((*out_new_str)->_data, content, content_length_ + 1);
    (*out_new_str)->_data[content_length_] = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_construct_char(const Char chr, String *out_new_str) {
    if (!chr) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize capacity_   = _container_string_apply_alignment(sizeof(Char) + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
    ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("Strings", alloc_size_, (VoidPtr *)out_new_str) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_capacity = capacity_;
    (*out_new_str)->_data     = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

    (*out_new_str)->_data[(*out_new_str)->_size++] = chr;
    (*out_new_str)->_data[(*out_new_str)->_size++] = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_construct_chars(const Char chr, const ByteSize count, String *out_new_str) {
    if (!chr || !count) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize content_length_ = sizeof(Char) * count;
    ByteSize capacity_       = _container_string_apply_alignment(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
    ByteSize alloc_size_     = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("Strings", alloc_size_, (VoidPtr *)out_new_str) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_size     = content_length_;
    (*out_new_str)->_capacity = capacity_;
    (*out_new_str)->_data     = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

    memset((*out_new_str)->_data, (Int32)chr, content_length_);
    (*out_new_str)->_data[content_length_] = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_construct_formatted(String *out_new_str, ConstStr format, ...) {
    if (!format) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize content_length_ = 0;
    {
        VaList va_list_;
        va_start(va_list_, format);
        content_length_ = vsnprintf(NULL, 0, format, va_list_);
        va_end(va_list_);
    }
    ByteSize capacity_   = _container_string_apply_alignment(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
    ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("Strings", alloc_size_, (VoidPtr *)out_new_str) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_size     = content_length_;
    (*out_new_str)->_capacity = capacity_;
    (*out_new_str)->_data     = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

    {
        VaList va_list_;
        va_start(va_list_, format);
        vsnprintf((*out_new_str)->_data, content_length_ + 1, format, va_list_);
        (*out_new_str)->_data[content_length_] = '\0';
        va_end(va_list_);
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_destruct(String str) {
    if (!str) return CONTAINER_ERROR_INVALID_PARAM;
    if (!str->_data || !str->_capacity) return CONTAINER_ERROR_NOT_ALLOCATED;

    if (memory_zone_deallocate("Strings", str, str->_capacity) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    memset(str, 0, sizeof(struct Container_String));
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_append(String *str, ConstStr content) {
    if (!content) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*str)) return container_string_construct(content, str);

    // handle container resizing
    ByteSize content_length_ = strlen(content);
    if ((*str)->_size + (content_length_ + 1) > (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (_container_string_apply_alignment(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

        ContainerResult resize_ = _container_string_resize(str, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    memcpy((*str)->_data + (*str)->_size, content, content_length_ + 1);
    (*str)->_size += content_length_;
    (*str)->_data[(*str)->_size] = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_append_char(String *str, const Char chr) {
    if (!chr) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*str)) return container_string_construct_char(chr, str);

    // handle container resizing
    if ((*str)->_size + (sizeof(Char) + 1) > (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (_container_string_apply_alignment(sizeof(Char) + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

        ContainerResult resize_ = _container_string_resize(str, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    (*str)->_data[(*str)->_size++] = chr;
    (*str)->_data[(*str)->_size]   = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_append_chars(String *str, const Char chr, const ByteSize count) {
    if (!chr || !count) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*str)) return container_string_construct_chars(chr, count, str);

    // handle container resizing
    ByteSize content_length_ = sizeof(Char) * count;
    if ((*str)->_size + (content_length_ + 1) > (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (_container_string_apply_alignment(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

        ContainerResult resize_ = _container_string_resize(str, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    memset((*str)->_data + (*str)->_size, (Int32)chr, content_length_);
    (*str)->_size += content_length_;
    (*str)->_data[(*str)->_size] = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_append_formatted(String *str, ConstStr format, ...) {
    if (!format) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize content_length_ = 0;
    {
        VaList va_list_;
        va_start(va_list_, format);
        content_length_ = vsnprintf(NULL, 0, format, va_list_);
        va_end(va_list_);
    }

    if (!(*str)) {
        ByteSize capacity_   = _container_string_apply_alignment(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
        ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

        if (memory_zone_allocate("Strings", alloc_size_, (VoidPtr *)str) != MEMORY_ZONE_SUCCESS)
            return CONTAINER_ERROR_ALLOCATION_FAILED;

        (*str)->_size     = 0;
        (*str)->_capacity = capacity_;
        (*str)->_data     = (Str)((UIntPtr)(*str) + sizeof(struct Container_String));

    } else {
        // handle container resizing
        if ((*str)->_size + (content_length_ + 1) > (*str)->_capacity) {
            ByteSize new_capacity_ = (*str)->_capacity + (_container_string_apply_alignment(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

            ContainerResult resize_ = _container_string_resize(str, new_capacity_);
            if (resize_ != CONTAINER_SUCCESS)
                return resize_;
        }
    }

    // append formatted string
    {
        VaList va_list_;
        va_start(va_list_, format);
        vsnprintf((*str)->_data + (*str)->_size, (*str)->_capacity - (*str)->_size, format, va_list_);
        va_end(va_list_);
    }

    (*str)->_size += content_length_;
    (*str)->_data[(*str)->_size] = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_detach(String *str) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str)->_size) return CONTAINER_ERROR_EMPTY_DATA;

    (*str)->_data[--(*str)->_size] = '\0';
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_detach_ranged(String *str, const ByteSize range) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str)->_size) return CONTAINER_ERROR_EMPTY_DATA;
    if (!range || (range > (*str)->_size)) return CONTAINER_ERROR_INVALID_PARAM;

    (*str)->_size -= range;
    (*str)->_data[(*str)->_size] = '\0';
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_filter_char(String *str, const Char chr) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str)->_size) return CONTAINER_ERROR_EMPTY_DATA;
    if (!chr) return CONTAINER_ERROR_INVALID_PARAM;

    Str      data_  = (*str)->_data;
    ByteSize size_  = (*str)->_size;
    ByteSize write_ = 0;
    ByteSize read_  = 0;

#if defined(__AVX2__)
    __m256i target256_ = _mm256_set1_epi8(chr);

    for (; read_ + 32 <= size_; read_ += 32) {
        __m256i chunk_     = _mm256_loadu_si256((__m256i *)(data_ + read_));  // 32 bytes
        __m256i mask_      = _mm256_cmpeq_epi8(chunk_, target256_);           // compare each chunk byte with chr
        Int32   mask_bits_ = _mm256_movemask_epi8(mask_);                     // return the bitmask for matching bytes

        // no matching bytes
        if (mask_bits_ == 0) {
            // copy everything
            _mm256_storeu_si256((__m256i *)(data_ + write_), chunk_);
            write_ += 32;
        }

        // otherwise
        else {
            // copy only the non-matching bytes
            for (Int32 i = 0; i < 32; ++i) {
                if (data_[read_ + i] != chr)
                    data_[write_++] = data_[read_ + i];
            }
        }
    }
#endif

#if defined(__SSE__)
    __m128i target128_ = _mm_set1_epi8(chr);

    for (; read_ + 16 <= size_; read_ += 16) {
        __m128i chunk_      = _mm_loadu_si128((__m128i *)(data_ + read_));  // 16 bytes
        __m128i mask_       = _mm_cmpeq_epi8(chunk_, target128_);           // compare each chunk byte with chr
        Int32   match_bits_ = _mm_movemask_epi8(mask_);                     // return the bitmask for matching bytes

        // no matching bytes
        if (match_bits_ == 0) {
            // copy everthing
            _mm_storeu_si128((__m128i *)(data_ + write_), chunk_);
            write_ += 16;
        }

        // otherwise
        else {
            // copy only the non-matching bytes
            for (Int32 i = 0; i < 16; ++i) {
                if (data_[read_ + i] != chr)
                    data_[write_++] = data_[read_ + i];
            }
        }
    }
#endif

    // process the remaining bytes
    while (read_ < size_) {
        if (data_[read_] != chr)
            data_[write_++] = data_[read_];

        ++read_;
    }

    data_[write_] = '\0';
    (*str)->_size = write_;

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_equals(String left, ConstStr right, const Bool case_sentitive, Bool *out_result) {
    if (!left) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!right || !out_result) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize right_length_ = strlen(right);
    if (left->_size != right_length_) return CONTAINER_ERROR_MISMATCHED_SIZES;

    // handle case-sentitive
    if (case_sentitive) {
        *out_result = !strcmp(left->_data, right);
        return CONTAINER_SUCCESS;
    }

    // handle case-insentitive
    ByteSize idx_     = 0;
    Bool     matched_ = true;

#if defined(__AVX2__)
    {
        __m256i chunk_left_, chunk_right_;
        __m256i lowercase_mask_ = _mm256_set1_epi8(0x20);

        // loop through 32 bytes
        while ((idx_ + 32 <= left->_size) && (idx_ + 32 <= right_length_)) {
            chunk_left_  = _mm256_loadu_si256((__m256i *)(left->_data + idx_));
            chunk_right_ = _mm256_loadu_si256((__m256i *)(right + idx_));

            // convert to lowercase
            chunk_left_  = _mm256_andnot_si256(_mm256_cmpgt_epi8(chunk_left_, _mm256_set1_epi8('Z')), lowercase_mask_);
            chunk_right_ = _mm256_andnot_si256(_mm256_cmpgt_epi8(chunk_right_, _mm256_set1_epi8('Z')), lowercase_mask_);

            // compare
            if (_mm256_testz_si256(chunk_left_, chunk_right_)) {
                matched_ = false;
                break;
            }

            idx_ += 32;
        }
    }
#endif
#if defined(__SSE__)
    {
        __m128i chunk_left_, chunk_right_;
        __m128i lowercase_mask_ = _mm_set1_epi8(0x20);

        // loop through 16 bytes
        while ((idx_ + 16 <= left->_size) && (idx_ + 16 <= right_length_)) {
            chunk_left_  = _mm_loadu_si128((__m128i *)(left->_data + idx_));
            chunk_right_ = _mm_loadu_si128((__m128i *)(right + idx_));

            // convert to lowercase
            chunk_left_  = _mm_andnot_si128(_mm_cmpgt_epi8(chunk_left_, _mm_set1_epi8('Z')), lowercase_mask_);
            chunk_right_ = _mm_andnot_si128(_mm_cmpgt_epi8(chunk_right_, _mm_set1_epi8('Z')), lowercase_mask_);

            // compare
            if (_mm_testz_si128(chunk_left_, chunk_right_)) {
                matched_ = false;
                break;
            }

            idx_ += 16;
        }
    }
#endif
    {
        // process the remaining bytes
        if ((idx_ < left->_size) && (idx_ < right_length_))
            matched_ = (case_sentitive) ? !strcmp(left->_data + idx_, right + idx_) : !strcmpi(left->_data + idx_, right + idx_);
    }

    *out_result = matched_;
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_equals_string(String left, String right, const Bool case_sensitive, Bool *out_result) {
    if (!left || !right) return CONTAINER_ERROR_NOT_ALLOCATED;
    return container_string_equals(left, container_string_get(right), case_sensitive, out_result);
}

ContainerResult container_string_contains(String str, ConstStr substr, const Bool case_sensitive, Bool *out_result) {
    if (!str) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!substr || !out_result) return CONTAINER_ERROR_INVALID_PARAM;
    if (!str->_size) return CONTAINER_ERROR_EMPTY_DATA;

    ByteSize str_length_    = str->_size;
    ByteSize substr_length_ = strlen(substr);
    if (substr_length_ > str_length_) return CONTAINER_ERROR_MISMATCHED_SIZES;

    if (case_sensitive) {
        *out_result = (strstr(str->_data, substr) != NULL);
        return CONTAINER_SUCCESS;
    }

    ByteSize idx_ = 0;

#if defined(__AVX2__)
    {
        __m256i lowercase_mask_ = _mm256_set1_epi8(0x20);
        __m256i z_              = _mm256_set1_epi8('Z');

        // loop through 32 bytes
        while (idx_ + 32 <= str_length_) {
            __m256i chunk_str_ = _mm256_loadu_si256((__m256i *)(str->_data + idx_));

            // convert uppercase bytes to lowercase
            __m256i is_uppercase_ = _mm256_cmpgt_epi8(chunk_str_, z_);
            chunk_str_            = _mm256_or_si256(chunk_str_, _mm256_andnot_si256(is_uppercase_, lowercase_mask_));

            // compare the first character of substr to string chunk
            __m256i substr_first_ = _mm256_set1_epi8(tolower((UInt8)substr[0]));
            __m256i cmp_mask_     = _mm256_cmpeq_epi8(chunk_str_, substr_first_);
            Int32   match_bits_   = _mm256_movemask_epi8(cmp_mask_);

            while (match_bits_) {
                Int32 matched_pos_ = _container_string_ctz(match_bits_);  // search for first position where the bits match

                // check from matched position
                ByteSize start_ = idx_ + matched_pos_;
                if (start_ + substr_length_ <= str_length_) {
                    if (!strncasecmp(str->_data + start_, substr, substr_length_)) {
                        *out_result = true;
                        return CONTAINER_SUCCESS;
                    }
                }

                match_bits_ &= (match_bits_ - 1);
            }

            idx_ += 32;
        }
    }
#endif

#if defined(__SSE__)
    {
        __m128i lowercase_mask_ = _mm_set1_epi8(0x20);
        __m128i z_              = _mm_set1_epi8('Z');

        // loop through 16 bytes
        while (idx_ + 16 <= str_length_) {
            __m128i chunk_str_ = _mm_loadu_si128((__m128i *)(str->_data + idx_));

            // convert uppercase bytes to lowercase
            __m128i is_uppercase_ = _mm_cmpgt_epi8(chunk_str_, z_);
            chunk_str_            = _mm_or_si128(chunk_str_, _mm_andnot_si128(is_uppercase_, lowercase_mask_));

            // compare the first character of substr to string chunk
            __m128i substr_first_ = _mm_set1_epi8(tolower((UInt8)substr[0]));
            __m128i cmp_mask_     = _mm_cmpeq_epi8(chunk_str_, substr_first_);
            Int32   match_bits_   = _mm_movemask_epi8(cmp_mask_);

            while (match_bits_) {
                Int32 matched_pos_ = _container_string_ctz(match_bits_);  // search for first position where the bits match

                // check from matched position
                ByteSize start_ = idx_ + matched_pos_;
                if (start_ + substr_length_ <= str_length_) {
                    if (!strncasecmp(str->_data + start_, substr, substr_length_)) {
                        *out_result = true;
                        return CONTAINER_SUCCESS;
                    }
                }

                match_bits_ &= (match_bits_ - 1);
            }

            idx_ += 16;
        }
    }
#endif

    for (; idx_ + substr_length_ <= str_length_; ++idx_) {
        if (tolower((UInt8)str->_data[idx_]) == tolower((UInt8)substr[0])) {
            if (strncasecmp(str->_data + idx_, substr, substr_length_) == 0) {
                *out_result = true;
                return CONTAINER_SUCCESS;
            }
        }
    }

    *out_result = false;
    return CONTAINER_SUCCESS;
}
Str container_string_get(String str) {
    return (!str) ? NULL : str->_data;
}

ByteSize container_string_size(String str) {
    return (!str) ? 0 : str->_size;
}

ByteSize container_string_capacity(String str) {
    return (!str) ? 0 : str->_capacity;
}
