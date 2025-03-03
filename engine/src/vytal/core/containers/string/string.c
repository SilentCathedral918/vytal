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

    // used for allocations/deallocations
    ByteSize _memory_size;
};

VYTAL_INLINE ContainerResult _container_string_resize(String *str, const ByteSize new_capacity) {
    ByteSize new_alloc_size_ = sizeof(struct Container_String) + new_capacity;

    String old_str_ = *str;
    String new_str_ = NULL;
    if (memory_zone_allocate("strings", new_alloc_size_, (VoidPtr *)&new_str_, NULL) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    new_str_->_size        = (*str)->_size;
    new_str_->_capacity    = new_capacity;
    new_str_->_memory_size = new_alloc_size_;
    new_str_->_data        = (Str)((UIntPtr)new_str_ + sizeof(struct Container_String));

    memcpy(new_str_->_data, (*str)->_data, (*str)->_size);

    if (memory_zone_deallocate("strings", old_str_, old_str_->_memory_size) != MEMORY_ZONE_SUCCESS)
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
    ByteSize capacity_   = VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
    ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("strings", alloc_size_, (VoidPtr *)out_new_str, NULL) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_size        = content_length_;
    (*out_new_str)->_capacity    = capacity_;
    (*out_new_str)->_memory_size = alloc_size_;
    (*out_new_str)->_data        = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

    memcpy((*out_new_str)->_data, content, content_length_ + 1);
    (*out_new_str)->_data[content_length_] = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_construct_char(const Char chr, String *out_new_str) {
    if (!chr) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize capacity_   = VYTAL_APPLY_ALIGNMENT(sizeof(Char) + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
    ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("strings", alloc_size_, (VoidPtr *)out_new_str, NULL) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_size        = 0;
    (*out_new_str)->_capacity    = capacity_;
    (*out_new_str)->_memory_size = alloc_size_;
    (*out_new_str)->_data        = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

    (*out_new_str)->_data[(*out_new_str)->_size++] = chr;
    (*out_new_str)->_data[(*out_new_str)->_size++] = '\0';

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_construct_chars(const Char chr, const ByteSize count, String *out_new_str) {
    if (!chr || !count) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize content_length_ = sizeof(Char) * count;
    ByteSize capacity_       = VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
    ByteSize alloc_size_     = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("strings", alloc_size_, (VoidPtr *)out_new_str, NULL) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_size        = content_length_;
    (*out_new_str)->_capacity    = capacity_;
    (*out_new_str)->_memory_size = alloc_size_;
    (*out_new_str)->_data        = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

    memset((*out_new_str)->_data, chr, content_length_);
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
    ByteSize capacity_   = VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
    ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("strings", alloc_size_, (VoidPtr *)out_new_str, NULL) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_size        = content_length_;
    (*out_new_str)->_capacity    = capacity_;
    (*out_new_str)->_memory_size = alloc_size_;
    (*out_new_str)->_data        = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

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
    if (!str->_data || !str->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    if (memory_zone_deallocate("strings", str, str->_memory_size) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    memset(str, 0, sizeof(struct Container_String));
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_append(String *str, ConstStr content) {
    if (!content) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*str)) return container_string_construct(content, str);

    // handle container resizing
    ByteSize content_length_ = strlen(content);
    if ((*str)->_size + (content_length_ + 1) >= (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

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
    if ((*str)->_size + (sizeof(Char) + 1) >= (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(sizeof(Char) + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

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
    if ((*str)->_size + (content_length_ + 1) >= (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

        ContainerResult resize_ = _container_string_resize(str, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    memset((*str)->_data + (*str)->_size, chr, content_length_);
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
        ByteSize capacity_   = VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
        ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

        if (memory_zone_allocate("strings", alloc_size_, (VoidPtr *)str, NULL) != MEMORY_ZONE_SUCCESS)
            return CONTAINER_ERROR_ALLOCATION_FAILED;

        (*str)->_size     = 0;
        (*str)->_capacity = capacity_;
        (*str)->_data     = (Str)((UIntPtr)(*str) + sizeof(struct Container_String));

    } else {
        // handle container resizing
        if ((*str)->_size + (content_length_ + 1) >= (*str)->_capacity) {
            ByteSize new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

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

ContainerResult container_string_clear(String *str) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str)->_size) return CONTAINER_ERROR_EMPTY_DATA;

    memset((*str)->_data, 0, (*str)->_size);
    (*str)->_size = 0;

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_equals(String left, ConstStr right, const Bool case_sentitive, Bool *out_result) {
    if (!left) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!right || !out_result) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize right_length_ = strlen(right);
    if (left->_size != right_length_) return CONTAINER_ERROR_SIZES_MISMATCHED;

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

ContainerResult container_string_search(String str, ConstStr substr, const Bool case_sensitive, Int32 *out_position) {
    if (!str) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!substr || !out_position) return CONTAINER_ERROR_INVALID_PARAM;
    if (!str->_size) return CONTAINER_ERROR_EMPTY_DATA;

    ByteSize str_length_    = str->_size;
    ByteSize substr_length_ = strlen(substr);
    if (substr_length_ > str_length_) return CONTAINER_ERROR_SIZES_MISMATCHED;

    ByteSize idx_ = 0;

#if defined(__AVX2__)
    {
        __m256i lowercase_mask_ = _mm256_set1_epi8(0x20);
        __m256i a_              = _mm256_set1_epi8('A');
        __m256i z_              = _mm256_set1_epi8('Z');

        // loop through 32 bytes
        while (idx_ + 32 <= str_length_) {
            __m256i chunk_str_ = _mm256_loadu_si256((__m256i *)(str->_data + idx_));

            // convert uppercase bytes to lowercase
            if (!case_sensitive) {
                __m256i is_uppercase_ = _mm256_and_si256(
                    _mm256_cmpgt_epi8(chunk_str_, _mm256_sub_epi8(a_, _mm256_set1_epi8(1))),
                    _mm256_cmpgt_epi8(z_, _mm256_sub_epi8(chunk_str_, _mm256_set1_epi8(1))));
                chunk_str_ = _mm256_or_si256(chunk_str_, _mm256_andnot_si256(is_uppercase_, lowercase_mask_));
            }

            // compare the first character of substr to string chunk
            __m256i substr_first_ = _mm256_set1_epi8(case_sensitive ? substr[0] : tolower((UInt8)substr[0]));
            __m256i cmp_mask_     = _mm256_cmpeq_epi8(chunk_str_, substr_first_);
            Int32   match_bits_   = _mm256_movemask_epi8(cmp_mask_);

            while (match_bits_) {
                Int32 matched_pos_ = _container_string_ctz(match_bits_);  // search for first position where the bits match

                // check from matched position
                ByteSize start_ = idx_ + matched_pos_;
                if (start_ + substr_length_ <= str_length_) {
                    if ((case_sensitive && !strncmp(str->_data + start_, substr, substr_length_)) ||
                        (!case_sensitive && !strncasecmp(str->_data + start_, substr, substr_length_))) {
                        *out_position = start_;
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
        __m128i a_              = _mm_set1_epi8('A');
        __m128i z_              = _mm_set1_epi8('Z');

        // loop through 16 bytes
        while (idx_ + 16 <= str_length_) {
            __m128i chunk_str_ = _mm_loadu_si128((__m128i *)(str->_data + idx_));

            // convert uppercase bytes to lowercase
            if (!case_sensitive) {
                __m128i is_uppercase_ = _mm_and_si128(
                    _mm_cmpgt_epi8(chunk_str_, _mm_sub_epi8(a_, _mm_set1_epi8(1))),
                    _mm_cmpgt_epi8(z_, _mm_sub_epi8(chunk_str_, _mm_set1_epi8(1))));
                chunk_str_ = _mm_or_si128(chunk_str_, _mm_andnot_si128(is_uppercase_, lowercase_mask_));
            }

            // compare the first character of substr to string chunk
            __m128i substr_first_ = _mm_set1_epi8(case_sensitive ? substr[0] : tolower((UInt8)substr[0]));
            __m128i cmp_mask_     = _mm_cmpeq_epi8(chunk_str_, substr_first_);
            Int32   match_bits_   = _mm_movemask_epi8(cmp_mask_);

            while (match_bits_) {
                Int32 matched_pos_ = _container_string_ctz(match_bits_);  // search for first position where the bits match

                // check from matched position
                ByteSize start_ = idx_ + matched_pos_;
                if (start_ + substr_length_ <= str_length_) {
                    if ((case_sensitive && !strncmp(str->_data + start_, substr, substr_length_)) ||
                        (!case_sensitive && !strncasecmp(str->_data + start_, substr, substr_length_))) {
                        *out_position = start_;
                        return CONTAINER_SUCCESS;
                    }
                }

                match_bits_ &= (match_bits_ - 1);
            }

            idx_ += 16;
        }
    }
#endif

    for (; idx_ < str_length_ - substr_length_; ++idx_) {
        if ((case_sensitive && str->_data[idx_] == substr[0]) ||
            (!case_sensitive && tolower((UInt8)str->_data[idx_]) == tolower((UInt8)substr[0]))) {
            if ((case_sensitive && !strncmp(str->_data + idx_, substr, substr_length_)) ||
                (!case_sensitive && !strncasecmp(str->_data + idx_, substr, substr_length_))) {
                *out_position = idx_;
                return CONTAINER_SUCCESS;
            }
        }
    }

    *out_position = -1;
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_contains(String str, ConstStr substr, const Bool case_sensitive, Bool *out_result) {
    if (!str) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!substr || !out_result) return CONTAINER_ERROR_INVALID_PARAM;
    if (!str->_size) return CONTAINER_ERROR_EMPTY_DATA;

    Int32           position_;
    ContainerResult search_string_ = container_string_search(str, substr, case_sensitive, &position_);
    if (search_string_ != CONTAINER_SUCCESS)
        return search_string_;

    *out_result = (position_ != -1);
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_begins_with(String str, ConstStr substr, const Bool case_sensitive, Bool *out_result) {
    if (!str) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!substr || !out_result) return CONTAINER_ERROR_INVALID_PARAM;
    if (!str->_size) return CONTAINER_ERROR_EMPTY_DATA;

    ByteSize substr_length_ = strlen(substr);
    if (substr_length_ > str->_size) return CONTAINER_ERROR_SIZES_MISMATCHED;

    *out_result = (case_sensitive)
                      ? (strncmp(str->_data, substr, substr_length_) == 0)
                      : (strncasecmp(str->_data, substr, substr_length_) == 0);

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_ends_with(String str, ConstStr substr, const Bool case_sensitive, Bool *out_result) {
    if (!str) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!substr || !out_result) return CONTAINER_ERROR_INVALID_PARAM;
    if (!str->_size) return CONTAINER_ERROR_EMPTY_DATA;

    ByteSize substr_length_ = strlen(substr);
    if (substr_length_ > str->_size) return CONTAINER_ERROR_SIZES_MISMATCHED;

    ConstStr end_ = str->_data + (str->_size - substr_length_);
    *out_result   = (case_sensitive)
                        ? (strncmp(end_, substr, substr_length_) == 0)
                        : (strncasecmp(end_, substr, substr_length_) == 0);

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_search_first_char(String str, const Char chr, Int32 *out_position) {
    if (!str) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!str->_size) return CONTAINER_ERROR_EMPTY_DATA;
    if (!out_position || !chr) return CONTAINER_ERROR_INVALID_PARAM;

    Str found_    = strchr(str->_data, chr);
    *out_position = !found_ ? -1 : (Int32)(found_ - str->_data);
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_search_last_char(String str, const Char chr, Int32 *out_position) {
    if (!str) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!str->_size) return CONTAINER_ERROR_EMPTY_DATA;
    if (!out_position || !chr) return CONTAINER_ERROR_INVALID_PARAM;

    Str found_    = strrchr(str->_data, chr);
    *out_position = !found_ ? -1 : (Int32)(found_ - str->_data);
    return CONTAINER_SUCCESS;
}

ContainerResult container_string_insert(String *str, const ByteSize index, ConstStr content) {
    if (!content || (index > (*str)->_size)) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*str) || !(*str)->_size) return container_string_append(str, content);

    ByteSize content_length_ = strlen(content);
    if (!content_length_) return CONTAINER_ERROR_INVALID_PARAM;

    // handle container resizing
    if ((*str)->_size + (content_length_ + 1) >= (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

        ContainerResult resize_ = _container_string_resize(str, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    // handle insertion
    {
        if (index != (*str)->_size)
            memmove((*str)->_data + index + content_length_, (*str)->_data + index, (*str)->_size - index);

        // slot in the content
        memcpy((*str)->_data + index, content, content_length_);

        // update size and null-terminate
        (*str)->_size += content_length_;
        (*str)->_data[(*str)->_size] = '\0';
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_insert_char(String *str, const ByteSize index, const Char chr) {
    if ((index > (*str)->_size)) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*str) || !(*str)->_size) return container_string_append_char(str, chr);

    ByteSize content_length_ = sizeof(Char);
    if (!content_length_) return CONTAINER_ERROR_INVALID_PARAM;

    // handle container resizing
    if ((*str)->_size + (content_length_ + 1) >= (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

        ContainerResult resize_ = _container_string_resize(str, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    // handle insertion
    {
        if (index != (*str)->_size)
            memmove((*str)->_data + index + content_length_, (*str)->_data + index, (*str)->_size - index);

        // slot in the character
        (*str)->_data[index] = chr;

        // update size and null-terminate
        (*str)->_size += content_length_;
        (*str)->_data[(*str)->_size] = '\0';
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_insert_chars(String *str, const ByteSize index, const Char chr, const ByteSize count) {
    if (!count || (index > (*str)->_size)) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*str) || !(*str)->_size) return container_string_append_chars(str, chr, count);

    ByteSize content_length_ = sizeof(Char) * count;
    if (!content_length_) return CONTAINER_ERROR_INVALID_PARAM;

    // handle container resizing
    if ((*str)->_size + (content_length_ + 1) >= (*str)->_capacity) {
        ByteSize new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

        ContainerResult resize_ = _container_string_resize(str, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    // handle insertion
    {
        if (index != (*str)->_size)
            memmove((*str)->_data + index + content_length_, (*str)->_data + index, (*str)->_size - index);

        // slot in the content
        memset((*str)->_data + index, chr, content_length_);

        // update size and null-terminate
        (*str)->_size += content_length_;
        (*str)->_data[(*str)->_size] = '\0';
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_insert_formatted(String *str, const ByteSize index, ConstStr format, ...) {
    if (!format || (index > (*str)->_size)) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize content_length_ = 0;
    {
        VaList va_list_;
        va_start(va_list_, format);
        content_length_ = vsnprintf(NULL, 0, format, va_list_);
        va_end(va_list_);
    }

    if (content_length_ <= 0) return CONTAINER_ERROR_INVALID_PARAM;

    if (!(*str)) {
        ByteSize capacity_   = VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR;
        ByteSize alloc_size_ = sizeof(struct Container_String) + capacity_;

        if (memory_zone_allocate("strings", alloc_size_, (VoidPtr *)str, NULL) != MEMORY_ZONE_SUCCESS)
            return CONTAINER_ERROR_ALLOCATION_FAILED;

        (*str)->_size     = 0;
        (*str)->_capacity = capacity_;
        (*str)->_data     = (Str)((UIntPtr)(*str) + sizeof(struct Container_String));
    } else {
        // handle container resizing
        if ((*str)->_size + (content_length_ + 1) >= (*str)->_capacity) {
            ByteSize new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);

            ContainerResult resize_ = _container_string_resize(str, new_capacity_);
            if (resize_ != CONTAINER_SUCCESS)
                return resize_;
        }
    }

    // handle insertion
    {
        if (index != (*str)->_size)
            memmove((*str)->_data + index + content_length_, (*str)->_data + index, (*str)->_size - index);

        // slot in the formatted content
        {
            VaList va_list_;
            va_start(va_list_, format);
            vsnprintf((*str)->_data + index, content_length_ + 1, format, va_list_);
            va_end(va_list_);
        }

        // update size and null-terminate
        (*str)->_size += content_length_;
        (*str)->_data[(*str)->_size] = '\0';
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_remove(String *str, const ByteSize index, const ByteSize length) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str)->_size) return CONTAINER_ERROR_EMPTY_DATA;
    if ((index >= (*str)->_size) || !length) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize actual_length_ = (index + length > (*str)->_size) ? ((*str)->_size - index) : length;

    // handle removal
    {
        // shift the trailing train to the left in <actual_length> characters
        memmove((*str)->_data + index, (*str)->_data + index + actual_length_, (*str)->_size - (index + actual_length_));

        // clear the right side of trailing train
        memset((*str)->_data + ((*str)->_size - actual_length_), 0, actual_length_);

        // update the size and null-terminate
        (*str)->_size -= actual_length_;
        (*str)->_data[(*str)->_size] = '\0';
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_to_lower(String *str) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str) || !(*str)->_size) return CONTAINER_ERROR_EMPTY_DATA;

    ByteSize idx_ = 0;

#if defined(__AVX2__)
    {
        __m256i lowercase_mask_ = _mm256_set1_epi8(0x20);
        __m256i a_              = _mm256_set1_epi8('A');
        __m256i z_              = _mm256_set1_epi8('Z');

        // loop through 32 bytes
        while (idx_ + 32 <= (*str)->_size) {
            // load in the chunk
            __m256i chunk_ = _mm256_loadu_si256((__m256i *)((*str)->_data + idx_));

            // convert to lowercase for bytes that need to
            __m256i is_uppercase_ = _mm256_and_si256(
                _mm256_cmpgt_epi8(chunk_, _mm256_sub_epi8(a_, _mm256_set1_epi8(1))),
                _mm256_cmpgt_epi8(z_, _mm256_sub_epi8(chunk_, _mm256_set1_epi8(1))));
            chunk_ = _mm256_or_si256(chunk_, _mm256_and_si256(is_uppercase_, lowercase_mask_));

            // save the chunk
            _mm256_storeu_si256((__m256i *)((*str)->_data + idx_), chunk_);
            idx_ += 32;
        }
    }
#endif
#if defined(__SSE__)
    {
        __m128i lowercase_mask_ = _mm_set1_epi8(0x20);
        __m128i a_              = _mm_set1_epi8('A');
        __m128i z_              = _mm_set1_epi8('Z');

        // loop through 16 bytes
        while (idx_ + 16 <= (*str)->_size) {
            // load in the chunk
            __m128i chunk_ = _mm_loadu_si128((__m128i *)((*str)->_data + idx_));

            // convert to lowercase for bytes that need to
            __m128i is_uppercase_ = _mm_and_si128(
                _mm_cmpgt_epi8(chunk_, _mm_sub_epi8(a_, _mm_set1_epi8(1))),
                _mm_cmpgt_epi8(z_, _mm_sub_epi8(chunk_, _mm_set1_epi8(1))));
            chunk_ = _mm_or_si128(chunk_, _mm_and_si128(is_uppercase_, lowercase_mask_));

            // save the chunk
            _mm_storeu_si128((__m128i *)((*str)->_data + idx_), chunk_);
            idx_ += 16;
        }
    }
#endif
    {
        // process the remaining bytes
        while (idx_ < (*str)->_size) {
            if ((*str)->_data[idx_] >= 'A' && (*str)->_data[idx_] <= 'Z')
                (*str)->_data[idx_] |= 0x20;

            ++idx_;
        }
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_to_upper(String *str) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str) || !(*str)->_size) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize idx_ = 0;

#if defined(__AVX2__)
    {
        __m256i uppercase_mask_ = _mm256_set1_epi8(~0x20);
        __m256i a_              = _mm256_set1_epi8('a');
        __m256i z_              = _mm256_set1_epi8('z');

        // loop through 32 bytes
        while (idx_ + 32 <= (*str)->_size) {
            // load in the chunk
            __m256i chunk_ = _mm256_loadu_si256((__m256i *)((*str)->_data + idx_));

            // convert to uppercase for bytes that need to
            __m256i is_lowercase_ = _mm256_and_si256(
                _mm256_cmpgt_epi8(chunk_, _mm256_sub_epi8(a_, _mm256_set1_epi8(1))),
                _mm256_cmpgt_epi8(z_, _mm256_sub_epi8(chunk_, _mm256_set1_epi8(1))));
            chunk_ = _mm256_or_si256(_mm256_and_si256(chunk_, _mm256_and_si256(is_lowercase_, uppercase_mask_)),
                                     _mm256_andnot_si256(is_lowercase_, chunk_));

            // save the chunk
            _mm256_storeu_si256((__m256i *)((*str)->_data + idx_), chunk_);
            idx_ += 32;
        }
    }
#endif
#if defined(__SSE__)
    {
        __m128i uppercase_mask_ = _mm_set1_epi8(~0x20);
        __m128i a_              = _mm_set1_epi8('a');
        __m128i z_              = _mm_set1_epi8('z');

        // loop through 16 bytes
        while (idx_ + 16 <= (*str)->_size) {
            // load in the chunk
            __m128i chunk_ = _mm_loadu_si128((__m128i *)((*str)->_data + idx_));

            // convert to uppercase for bytes that need to
            __m128i is_lowercase_ = _mm_and_si128(
                _mm_cmpgt_epi8(chunk_, _mm_sub_epi8(a_, _mm_set1_epi8(1))),
                _mm_cmpgt_epi8(z_, _mm_sub_epi8(chunk_, _mm_set1_epi8(1))));
            chunk_ = _mm_or_si128(_mm_and_si128(chunk_, _mm_and_si128(is_lowercase_, uppercase_mask_)),
                                  _mm_andnot_si128(is_lowercase_, chunk_));

            // save the chunk
            _mm_storeu_si128((__m128i *)((*str)->_data + idx_), chunk_);
            idx_ += 16;
        }
    }
#endif
    {
        // process the remaining bytes
        while (idx_ < (*str)->_size) {
            if ((*str)->_data[idx_] >= 'a' && (*str)->_data[idx_] <= 'z')
                (*str)->_data[idx_] &= ~0x20;

            ++idx_;
        }
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_trim(String *str) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str) || !(*str)->_size) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize start_ = 0, end_ = (*str)->_size;

#if defined(__AVX2__)
    {
        __m256i space_mask_ = _mm256_set1_epi8(' ');
        __m256i tab_mask_   = _mm256_set1_epi8('\t');

        // trim leading spaces
        while (start_ + 32 <= end_) {
            __m256i chunk_    = _mm256_loadu_si256((__m256i *)((*str)->_data + start_));
            __m256i is_space_ = _mm256_or_si256(
                _mm256_cmpeq_epi8(chunk_, space_mask_),
                _mm256_cmpeq_epi8(chunk_, tab_mask_));

            Int32 mask_ = _mm256_movemask_epi8(is_space_);

            // if not all characters are spaces
            if (mask_ != 0xffffffff) {
                start_ += _container_string_ctz(~mask_);  // find the first non-space
                break;
            }

            start_ += 32;
        }

        // trim trailing spaces
        while (end_ >= 32) {
            __m256i chunk_    = _mm256_loadu_si256((__m256i *)((*str)->_data + end_ - 32));
            __m256i is_space_ = _mm256_or_si256(
                _mm256_cmpeq_epi8(chunk_, space_mask_),
                _mm256_cmpeq_epi8(chunk_, tab_mask_));

            Int32 mask_ = _mm256_movemask_epi8(is_space_);

            // if not all characters are spaces
            if (mask_ != 0xffffffff) {
                end_ -= _container_string_ctz(~mask_);  // find the last non-space
                break;
            }

            end_ -= 32;
        }
    }
#endif
#if defined(__SSE__) && !defined(__AVX2__)
    {
        __m128i space_mask_ = _mm_set1_epi8(' ');
        __m128i tab_mask_   = _mm_set1_epi8('\t');

        // trim leading spaces
        while (start_ + 16 <= end_) {
            __m128i chunk_    = _mm_loadu_si128((__m128i *)((*str)->_data + start_));
            __m128i is_space_ = _mm_or_si128(
                _mm_cmpeq_epi8(chunk_, space_mask_),
                _mm_cmpeq_epi8(chunk_, tab_mask_));

            Int32 mask_ = _mm_movemask_epi8(is_space_);

            // if not all characters are spaces
            if (mask_ != 0xffff) {
                start_ += _container_string_ctz(~mask_);  // find the first non-space
                break;
            }

            start_ += 16;
        }

        // trim trailing spaces
        while (end_ >= 16) {
            __m128i chunk_    = _mm_loadu_si128((__m128i *)((*str)->_data + end_ - 16));
            __m128i is_space_ = _mm_or_si128(
                _mm_cmpeq_epi8(chunk_, space_mask_),
                _mm_cmpeq_epi8(chunk_, tab_mask_));

            Int32 mask_ = _mm_movemask_epi8(is_space_);

            // if not all characters are spaces
            if (mask_ != 0xffff) {
                end_ -= _container_string_ctz(~mask_);  // find the first non-space
                break;
            }

            end_ -= 16;
        }
    }
#endif
    {
        while ((start_ < end_) && (((*str)->_data[start_] == ' ') || ((*str)->_data[start_] == '\t')))
            ++start_;

        while ((end_ > start_) && (((*str)->_data[end_ - 1] == ' ') || ((*str)->_data[end_ - 1] == '\t')))
            --end_;

        // shift the string if trimmed
        if (start_ > 0)
            memmove((*str)->_data, (*str)->_data + start_, end_ - start_);

        // update size and null-terminate
        (*str)->_size                = end_ - start_;
        (*str)->_data[(*str)->_size] = '\0';
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_trim_left(String *str) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str) || !(*str)->_size) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize start_ = 0, end_ = (*str)->_size;

#if defined(__AVX2__)
    {
        __m256i space_mask_ = _mm256_set1_epi8(' ');
        __m256i tab_mask_   = _mm256_set1_epi8('\t');

        while (start_ + 32 <= end_) {
            __m256i chunk_    = _mm256_loadu_si256((__m256i *)((*str)->_data + start_));
            __m256i is_space_ = _mm256_or_si256(
                _mm256_cmpeq_epi8(chunk_, space_mask_),
                _mm256_cmpeq_epi8(chunk_, tab_mask_));

            Int32 mask_ = _mm256_movemask_epi8(is_space_);

            // if not all characters are spaces
            if (mask_ != 0xffffffff) {
                start_ += _container_string_ctz(~mask_);  // find the first non-space
                break;
            }

            start_ += 32;
        }
    }
#endif
#if defined(__SSE__) && !defined(__AVX2__)
    {
        __m128i space_mask_ = _mm_set1_epi8(' ');
        __m128i tab_mask_   = _mm_set1_epi8('\t');

        while (start_ + 16 <= end_) {
            __m128i chunk_    = _mm_loadu_si128((__m128i *)((*str)->_data + start_));
            __m128i is_space_ = _mm_or_si128(
                _mm_cmpeq_epi8(chunk_, space_mask_),
                _mm_cmpeq_epi8(chunk_, tab_mask_));

            Int32 mask_ = _mm_movemask_epi8(is_space_);

            // if not all characters are spaces
            if (mask_ != 0xffff) {
                start_ += _container_string_ctz(~mask_);  // find the first non-space
                break;
            }

            start_ += 16;
        }
    }
#endif
    {
        while ((start_ < end_) && (((*str)->_data[start_] == ' ') || ((*str)->_data[start_] == '\t')))
            ++start_;

        // shift the string if trimmed
        if (start_ > 0)
            memmove((*str)->_data, (*str)->_data + start_, end_ - start_);

        // update size and null-terminate
        (*str)->_size                = end_ - start_;
        (*str)->_data[(*str)->_size] = '\0';
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_trim_right(String *str) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str) || !(*str)->_size) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize start_ = 0, end_ = (*str)->_size;

#if defined(__AVX2__)
    {
        __m256i space_mask_ = _mm256_set1_epi8(' ');
        __m256i tab_mask_   = _mm256_set1_epi8('\t');

        while (end_ >= 32) {
            __m256i chunk_    = _mm256_loadu_si256((__m256i *)((*str)->_data + end_ - 32));
            __m256i is_space_ = _mm256_or_si256(
                _mm256_cmpeq_epi8(chunk_, space_mask_),
                _mm256_cmpeq_epi8(chunk_, tab_mask_));

            Int32 mask_ = _mm256_movemask_epi8(is_space_);

            // if not all characters are spaces
            if (mask_ != 0xffffffff) {
                end_ -= _container_string_ctz(~mask_);  // find the last non-space
                break;
            }

            end_ -= 32;
        }
    }
#endif
#if defined(__SSE__) && !defined(__AVX2__)
    {
        __m128i space_mask_ = _mm_set1_epi8(' ');
        __m128i tab_mask_   = _mm_set1_epi8('\t');

        while (end_ >= 16) {
            __m128i chunk_    = _mm_loadu_si128((__m128i *)((*str)->_data + end_ - 16));
            __m128i is_space_ = _mm_or_si128(
                _mm_cmpeq_epi8(chunk_, space_mask_),
                _mm_cmpeq_epi8(chunk_, tab_mask_));

            Int32 mask_ = _mm_movemask_epi8(is_space_);

            // if not all characters are spaces
            if (mask_ != 0xffff) {
                end_ -= _container_string_ctz(~mask_);  // find the first non-space
                break;
            }

            end_ -= 16;
        }
    }
#endif
    {
        while ((end_ > start_) && (((*str)->_data[end_ - 1] == ' ') || ((*str)->_data[end_ - 1] == '\t')))
            --end_;

        // update size and null-terminate
        (*str)->_size                = end_ - start_;
        (*str)->_data[(*str)->_size] = '\0';
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_replace(String *str, ConstStr old_substr, ConstStr new_substr) {
    if (!(*str)) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*str)->_size) return CONTAINER_ERROR_EMPTY_DATA;
    if (!old_substr || !new_substr) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize old_length_ = strlen(old_substr);
    ByteSize new_length_ = strlen(new_substr);
    if (!old_length_) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize  count_     = 0;
    ConstStr  curr_      = (*str)->_data;
    ByteSize *positions_ = NULL;

    while ((curr_ = strstr(curr_, old_substr)) != NULL) {
        positions_         = realloc(positions_, sizeof(ByteSize) * (count_ + 1));
        positions_[count_] = curr_ - (*str)->_data;

        ++count_;
        curr_ += old_length_;
    }

    // no occurences of old substrings -> no replacements needed
    if (!count_) {
        free(positions_);
        return CONTAINER_SUCCESS;
    }

    // handle container resizing
    ByteSize new_size_ = (*str)->_size + (new_length_ - old_length_) * count_;
    if (new_size_ >= (*str)->_capacity) {
        ByteSize        new_capacity_ = (*str)->_capacity + (VYTAL_APPLY_ALIGNMENT(new_size_ + 1, MEMORY_ALIGNMENT_SIZE) * CONTAINER_RESIZE_FACTOR);
        ContainerResult resize_       = _container_string_resize(str, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS) return resize_;
    }

    // replace all occurences
    for (ByteSize i = 0; i < count_; i++) {
        ByteSize pos_ = positions_[i] + (new_length_ - old_length_) * i;

        // shift data only if new string is larger
        if (new_length_ > old_length_)
            memmove(
                (*str)->_data + pos_ + new_length_,
                (*str)->_data + pos_ + old_length_,
                ((*str)->_size + (new_length_ - old_length_) * count_) - (pos_ + old_length_));

        // replace copy the new substring
        memcpy((*str)->_data + pos_, new_substr, new_length_);
    }

    // update size and null-terminate
    (*str)->_size            = new_size_;
    (*str)->_data[new_size_] = '\0';

    free(positions_);
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

Bool container_string_empty(String str) {
    return (!str) ? true : (!str->_size);
}
