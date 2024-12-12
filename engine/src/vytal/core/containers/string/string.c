#include "string.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/managers/memory/memmgr.h"

#include <ctype.h>
#include <emmintrin.h>
#include <stdarg.h>

#define CHAR_ARRAY_MAX_LENGTH 16384 // 16 KB

typedef struct Container_String_Data {
    ByteSize _length;
    ByteSize _capacity;
    Str      _str;
} StringData;

VT_INLINE StringData *_container_string_get_internal_data(String str) {
    return (!str || !(str->_internal_data)) ? NULL : (str->_internal_data);
}

String container_string_construct(ConstStr input) {
    ByteSize input_length_ = misc_str_strlen(input);

    // size set to 0 since 'containers' use pool allocator, where size is already determined
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_CONTAINERS);
    if (!chunk_)
        return NULL;

    // there would be three elements...

    // element 1: the string container self
    String string_ = VT_CAST(String, chunk_);

    // element 2: the string internal data struct
    StringData *data_  = VT_CAST(StringData *, string_ + 1);
    data_->_length     = input_length_;
    ByteSize capacity_ = 1;
    for (; capacity_ < input_length_; capacity_ *= 2) {
    }
    data_->_capacity = capacity_;

    // element 3: the raw string data
    data_->_str = VT_CAST(Str, data_ + 1);
    misc_str_strcpy(data_->_str, input);

    // assign internal data to string ownership
    string_->_internal_data = data_;

    return string_;
}

String container_string_construct_char(const Char c, const ByteSize count) {
    if (count == 0)
        return NULL;

    // size set to 0 since 'containers' use pool allocator, where size is already determined
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_CONTAINERS);
    if (!chunk_)
        return NULL;

    // there would be three elements...

    // element 1: the string container self
    String string_ = VT_CAST(String, chunk_);

    // element 2: the string internal data struct
    StringData *data_  = VT_CAST(StringData *, string_ + 1);
    data_->_length     = count;
    ByteSize capacity_ = 1;
    for (; capacity_ < count; capacity_ *= 2) {
    }
    data_->_capacity = capacity_;

    // element 3: the raw string data
    data_->_str = VT_CAST(Str, data_ + 1);
    hal_mem_memset(data_->_str, c, count);

    // assign internal data to string ownership
    string_->_internal_data = data_;

    return string_;
}

String container_string_construct_formatted(ConstStr format, ...) {
    if (!format)
        return NULL;

    Char value_[CHAR_ARRAY_MAX_LENGTH] = {'\0'};

    // format string value
    {
        VaList va_list_;
        va_start(va_list_, format);
        vsnprintf(value_, CHAR_ARRAY_MAX_LENGTH, format, va_list_);
        va_end(va_list_);
    }

    // pass the value over to default construct function to handle the construction
    return container_string_construct(value_);
}

Bool container_string_destruct(String str) {
    if (!str || !(str->_internal_data))
        return false;

    // free the entire string chunk
    {
        memory_manager_deallocate(str, MEMORY_TAG_CONTAINERS);
        str = NULL;
    }

    return true;
}

Bool container_string_append(String str, ConstStr input) {
    if (!str || !input)
        return false;

    // update internal data members
    ByteSize    input_length_ = misc_str_strlen(input);
    StringData *data_         = _container_string_get_internal_data(str);
    data_->_length += input_length_;
    ByteSize capacity_ = data_->_capacity;
    for (; capacity_ < data_->_length; capacity_ *= 2) {
    }
    data_->_capacity = capacity_;

    // update string
    data_->_str = misc_str_strcat(data_->_str, input);

    return true;
}

Bool container_string_append_char(String str, const Char c) {
    if (!str)
        return false;

    StringData *data_ = _container_string_get_internal_data(str);

    // append the character to end of string
    data_->_str[data_->_length++] = c;

    // update internal data members
    {
        ByteSize capacity_ = data_->_capacity;
        for (; capacity_ < data_->_length; capacity_ *= 2) {
        }
        data_->_capacity = capacity_;
    }

    return true;
}

Bool container_string_append_chars(String str, const Char c, const ByteSize count) {
    if (!str)
        return false;

    StringData *data_ = _container_string_get_internal_data(str);

    // append train of characters at end of string
    hal_mem_memset(data_->_str + data_->_length, c, count);

    // update internal data members
    data_->_length += count;
    ByteSize capacity_ = data_->_capacity;
    for (; capacity_ < data_->_length; capacity_ *= 2) {
    }
    data_->_capacity = capacity_;

    return true;
}

Bool container_string_append_formatted(String str, ConstStr format, ...) {
    if (!str)
        return false;

    Char  value_[CHAR_ARRAY_MAX_LENGTH] = {'\0'};
    Int32 formatted_length_             = 0;

    // format string value
    {
        VaList va_list_;
        va_start(va_list_, format);
        formatted_length_ = vsnprintf(value_, CHAR_ARRAY_MAX_LENGTH, format, va_list_);
        va_end(va_list_);
    }

    // append them to end of string
    {
        StringData *data_ = _container_string_get_internal_data(str);
        data_->_length += formatted_length_;
        ByteSize capacity_ = data_->_capacity;
        for (; capacity_ < data_->_length; capacity_ *= 2) {
        }
        data_->_capacity = capacity_;
        data_->_str      = misc_str_strcat(data_->_str, value_);
    }

    return true;
}

Bool container_string_detach(String str) {
    if (!str)
        return false;

    StringData *data_             = _container_string_get_internal_data(str);
    data_->_str[data_->_length--] = '\0';

    return true;
}

Bool container_string_filter_char(String str, const Char c) {
    if (!str || container_string_isempty(str) || !c)
        return false;

    Int32       idx_   = 0;
    StringData *data_  = _container_string_get_internal_data(str);
    Str         pstr_  = data_->_str;
    Str         occur_ = NULL;

    // go through the string, look for occurences of the character and remove them
    while ((occur_ = misc_str_strchr(pstr_, c)) != NULL) {
        data_->_str[idx_++] = *occur_;
        pstr_               = occur_ + 1;
        --data_->_length;
    }

    // null-terminate the string
    data_->_str[idx_] = '\0';

    return true;
}

Int32 container_string_compare(String left, ConstStr right, const Bool sensitive) {
    return misc_str_strcmp(container_string_get(left), right, sensitive);
}

Int32 container_string_compare_string(String left, String right, const Bool sensitive) {
    return misc_str_strcmp(container_string_get(left), container_string_get(right), sensitive);
}

Bool container_string_contains(String str, ConstStr substr, const Bool sensitive) {
    if (!str || container_string_isempty(str) || !substr)
        return false;

    return (misc_str_strstr(_container_string_get_internal_data(str)->_str, substr, sensitive) != NULL);
}

Bool container_string_beginswith(String str, ConstStr substr, const Bool sensitive) {

    if (!str || container_string_isempty(str) || !substr)
        return false;

    ByteSize    substr_length_ = misc_str_strlen(substr);
    StringData *data_          = _container_string_get_internal_data(str);

    if (substr_length_ > data_->_length)
        return false;

    return (misc_str_strncmp(data_->_str, substr, substr_length_, sensitive) == 0);
}

Bool container_string_endswith(String str, ConstStr substr, const Bool sensitive) {
    if (!str || container_string_isempty(str) || !substr)
        return false;

    ByteSize    substr_length_ = misc_str_strlen(substr);
    StringData *data_          = _container_string_get_internal_data(str);

    if (substr_length_ > data_->_length)
        return false;

    Str pcheck_ = data_->_str + data_->_length - substr_length_;
    return (misc_str_strncmp(pcheck_, substr, substr_length_, sensitive) == 0);
}

Bool container_string_clear(String str) {
    if (!str || container_string_isempty(str))
        return false;

    StringData *data_ = _container_string_get_internal_data(str);
    hal_mem_memzero(data_->_str, data_->_length);
    data_->_length = 0;

    return true;
}

Int32 container_string_search(String str, ConstStr substr, const Bool sensitive) {
    if (!str || container_string_isempty(str) || !substr)
        return false;

    ByteSize    substr_length_ = misc_str_strlen(substr);
    StringData *data_          = _container_string_get_internal_data(str);

    if (substr_length_ > data_->_length)
        return false;

    return VT_CAST(Int32, VT_CAST(PtrDiff, misc_str_strstr(data_->_str, substr, sensitive) - data_->_str));
}

Int32 container_string_search_firstchar(String str, const Char c) {
    if (!str || container_string_isempty(str) || !c)
        return false;

    StringData *data_ = _container_string_get_internal_data(str);
    return VT_CAST(Int32, VT_CAST(PtrDiff, misc_str_strchr(data_->_str, c) - data_->_str));
}

Int32 container_string_search_lastchar(String str, const Char c) {
    if (!str || container_string_isempty(str) || !c)
        return false;

    StringData *data_ = _container_string_get_internal_data(str);
    return VT_CAST(Int32, VT_CAST(PtrDiff, misc_str_strrchr(data_->_str, c) - data_->_str));
}

Bool container_string_insert_at(String str, ConstStr input, const ByteSize index) {
    if (!str || !input || !container_string_index_valid(str, index))
        return false;

    ByteSize    input_length_ = misc_str_strlen(input);
    StringData *data_         = _container_string_get_internal_data(str);

    if (input_length_ == 0)
        return false;

    // perform insertion...
    {
        // shift the trailing train to the right in <input_length> characters
        BytePtr pinsert_    = VT_CAST(BytePtr, VT_CAST(UIntPtr, data_->_str) + index);
        BytePtr pshiftdest_ = VT_CAST(BytePtr, VT_CAST(UIntPtr, pinsert_) + input_length_);
        hal_mem_memmove(pshiftdest_, pinsert_, data_->_length - index);

        // copy the input into insert point
        misc_str_strncpy(VT_CAST(Str, pinsert_), input, input_length_);
    }

    // update internal data members
    {
        data_->_length += input_length_;
        ByteSize capacity_ = data_->_capacity;
        for (; capacity_ < data_->_length; capacity_ *= 2) {
        }
        data_->_capacity = capacity_;
    }

    return true;
}

Bool container_string_insert_char_at(String str, const Char c, const ByteSize index) {
    if (!str || !container_string_index_valid(str, index) || !c)
        return false;

    StringData *data_ = _container_string_get_internal_data(str);

    if (index == data_->_length - 1)
        return container_string_append_char(str, c);

    // perform insertion...
    {
        // shift the trailing train to the right in <input_length> characters
        BytePtr pinsert_    = VT_CAST(BytePtr, VT_CAST(UIntPtr, data_->_str) + index);
        BytePtr pshiftdest_ = VT_CAST(BytePtr, VT_CAST(UIntPtr, pinsert_) + 1);
        hal_mem_memmove(pshiftdest_, pinsert_, 1);

        // assign the character into insert point
        *pinsert_ = c;
    }

    // update internal data members
    {
        ++data_->_length;
        ByteSize capacity_ = data_->_capacity;
        for (; capacity_ < data_->_length; capacity_ *= 2) {
        }
        data_->_capacity = capacity_;
    }

    return true;
}

Bool container_string_remove_at(String str, const ByteSize index, const ByteSize count) {
    if (!str || !container_string_index_valid(str, index) || (count == 0))
        return false;

    StringData *data_       = _container_string_get_internal_data(str);
    ByteSize    new_length_ = data_->_length - VT_CAST(ByteSize, count);

    if (count > (VT_CAST(ByteSize, data_->_length) - index))
        return false;

    // perform removal...
    {
        // shift the trailing train to the left in <input_length> characters
        BytePtr pshiftdest_ = VT_CAST(BytePtr, VT_CAST(UIntPtr, data_->_str) + index);
        BytePtr ptrail_     = VT_CAST(BytePtr, VT_CAST(UIntPtr, pshiftdest_) + count);
        hal_mem_memmove(pshiftdest_, ptrail_, data_->_length - (index + count));

        // null-terminate
        hal_mem_memzero(data_->_str + new_length_, data_->_length - (index + count));
    }

    // update internal data member length
    data_->_length = new_length_;

    return true;
}

Bool container_string_remove_char_at(String str, const Char c, const ByteSize index) {
    if (!str || !container_string_index_valid(str, index) || !c)
        return false;

    StringData *data_       = _container_string_get_internal_data(str);
    ByteSize    new_length_ = data_->_length - 1;

    if (index == data_->_length - 1)
        return container_string_detach(str);

    // perform removal...
    {
        // shift the trailing train to the left in <input_length> characters
        BytePtr pshiftdest_ = VT_CAST(BytePtr, VT_CAST(UIntPtr, data_->_str) + index);
        BytePtr ptrail_     = VT_CAST(BytePtr, VT_CAST(UIntPtr, pshiftdest_) + 1);
        hal_mem_memmove(pshiftdest_, ptrail_, data_->_length - (index + 1));

        // null-terminate
        data_->_str[new_length_] = '\0';
    }

    // update internal data member length
    data_->_length = new_length_;

    return true;
}

Bool container_string_replace(String str, ConstStr old, ConstStr new, const Bool sensitive) {
    if (!str || container_string_isempty(str) || !old || !new)
        return false;

    StringData *data_              = _container_string_get_internal_data(str);
    ByteSize    old_substr_length_ = misc_str_strlen(old);

    // perform replacement...
    {
        BytePtr pstr_     = VT_CAST(BytePtr, data_->_str);
        BytePtr preplace_ = NULL;

        while ((preplace_ = VT_CAST(BytePtr, misc_str_strstr(VT_CAST(Str, pstr_), old, sensitive))) != NULL) {
            Int32 idx_replace_ = VT_CAST(Int32, VT_CAST(PtrDiff, preplace_ - VT_CAST(BytePtr, data_->_str)));

            container_string_remove_at(str, idx_replace_, old_substr_length_);
            container_string_insert_at(str, new, idx_replace_);

            // move on to find next occurence
            pstr_ = preplace_ + 1;
        }
    }

    return true;
}

Bool container_string_replace_char(String str, const Char old, const Char new) {
    if (!str || container_string_isempty(str) || !old || !new)
        return false;

    StringData *data_ = _container_string_get_internal_data(str);

    // perform replacement...
    {
        BytePtr pstr_     = VT_CAST(BytePtr, data_->_str);
        BytePtr preplace_ = NULL;

        while ((preplace_ = VT_CAST(BytePtr, misc_str_strchr(VT_CAST(Str, pstr_), old))) != NULL) {
            *preplace_ = new;

            // move on to find next occurence
            pstr_ = preplace_ + 1;
        }
    }

    return true;
}

#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
#    define REGISTER_SIZE 16

// SSE2 doesn't support _mm_shuffle_epi8 directly, so we will make do with this func
__m128i _container_string_reverse_bytes_sse2(__m128i vec) {
    const __m128i mask_ = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);

    UInt8 temp_[16];
    _mm_storeu_si128(VT_CAST(__m128i *, temp_), vec);

    __m128i res_ = _mm_setzero_si128();
    for (ByteSize i = 0; i < 16; ++i)
        VT_CAST(UInt8 *, &res_)[i] = temp_[VT_CAST(UInt8 *, &mask_)[i]];

    return res_;
}

Bool container_string_reverse(String str) {
    if (!str || container_string_isempty(str))
        return false;

    StringData *data_ = _container_string_get_internal_data(str);

    BytePtr pbegin_ = VT_CAST(BytePtr, data_->_str);
    BytePtr pend_   = pbegin_ + data_->_length - 1;

    while (pend_ - pbegin_ >= 15) {
        __m128i begin_ = _mm_loadu_si128(VT_CAST(__m128i *, pbegin_));
        __m128i end_   = _mm_loadu_si128(VT_CAST(__m128i *, pend_ - 15));

        begin_ = _container_string_reverse_bytes_sse2(begin_);
        end_   = _container_string_reverse_bytes_sse2(end_);

        // Swap the reversed blocks
        _mm_storeu_si128(VT_CAST(__m128i *, pbegin_), end_);
        _mm_storeu_si128(VT_CAST(__m128i *, pend_ - 15), begin_);

        pbegin_ += 16;
        pend_ -= 16;
    }

    return true;
}

Str _container_string_lowercase_sse2(Str target) {
    ByteSize target_length_ = misc_str_strlen(target);
    __m128i  lower_mask_    = _mm_set1_epi8(0x20);
    UIntPtr  ptarget_head_  = VT_CAST(UIntPtr, target);

    while (target_length_ >= REGISTER_SIZE) {
        __m128i chunk_ = _mm_loadu_si128(VT_CAST(__m128i const *, target));
        __m128i res_   = _mm_or_si128(chunk_, lower_mask_);
        _mm_storeu_si128(VT_CAST(__m128i *, target), res_);

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

Str _container_string_uppercase_sse2(Str target) {
    ByteSize target_length_ = misc_str_strlen(target);
    __m128i  lower_bound_   = _mm_set1_epi8(0x61); // 'a' = 0x61
    __m128i  upper_bound_   = _mm_set1_epi8(0x7a); // 'z' = 0x7a
    __m128i  clear_mask_    = _mm_set1_epi8(0x20);

    ByteSize idx_ = 0;

    // process 16 bytes at once
    for (; idx_ + 15 < target_length_; idx_ += 16) {
        __m128i chunk_      = _mm_loadu_si128(VT_CAST(__m128i *, &target[idx_]));
        __m128i in_range_   = _mm_and_si128(_mm_cmpgt_epi8(chunk_, lower_bound_), _mm_cmplt_epi8(chunk_, upper_bound_));
        __m128i trans_mask_ = _mm_and_si128(in_range_, clear_mask_);
        __m128i res_        = _mm_andnot_si128(trans_mask_, chunk_);

        // write the transformed result back
        _mm_storeu_si128(VT_CAST(__m128i *, &target[idx_]), res_);
    }

    // handle remaining bytes
    for (; idx_ < target_length_; ++idx_) {
        if (target[idx_] >= 'a' && target[idx_] <= 'z') {
            target[idx_] &= ~0x20; // Clear the 6th bit
        }
    }

    return target;
}

Bool container_string_lowercase(String str) {
    if (!str || container_string_isempty(str))
        return false;

    StringData *data_ = _container_string_get_internal_data(str);
    data_->_str       = _container_string_lowercase_sse2(data_->_str);

    return true;
}

Bool container_string_uppercase(String str) {
    if (!str || container_string_isempty(str))
        return false;

    StringData *data_ = _container_string_get_internal_data(str);
    data_->_str       = _container_string_uppercase_sse2(data_->_str);

    return true;
}

#else
Bool container_string_reverse(String str) {
    if (!str || container_string_isempty(str))
        return false;

    StringData *data_ = _container_string_get_internal_data(str);

    BytePtr pbegin_ = VT_CAST(BytePtr, data_->_str);
    BytePtr pend_   = pbegin_ + data_->_length - 1;

    while (pend_ - pbegin_ >= 15) {
        UInt64 *pbegin64_ = VT_CAST(UInt64 *, pbegin_);
        UInt64 *pend64_   = VT_CAST(UInt64 *, pend_ - 7);

        UInt64 low_  = *pbegin64_;
        UInt64 high_ = *pend64_;

        // reverse bytes within the 64-bit chunks
        low_  = VT_BSWAP64(low_);
        high_ = VT_BSWAP64(high_);

        // swap the reversed chunks
        *pbegin64_ = high_;
        *pend64_   = low_;

        pbegin_ += 16;
        pend_ -= 16;
    }

    // Handle remaining bytes (less than 16)
    while (pbegin_ < pend_) {
        Char temp_ = *pbegin_;
        *pbegin_++ = *pend_;
        *pend_--   = temp_;
    }

    return true;
}

Str _container_string_lowercase_batched(Str target) {
    ByteSize target_length_ = misc_str_strlen(target);
    ByteSize idx_           = 0;
    UInt8    lower_mask_8_  = 0x20;
    UInt64   lower_mask_64_ = 0x2020202020202020ull;

// int128 is supported
#    if defined(__SIZEOF_INT128__)
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
#    else
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

#    endif
}

VT_INLINE UInt64 _container_string_range_check_batched(UInt64 chunk, UInt64 lower_bound, UInt64 upper_bound) {
    return ((~((chunk - lower_bound) | ((upper_bound - lower_bound) - (chunk - lower_bound))) & 0x8080808080808080) >> 7);
}

Str _container_string_uppercase_batched(Str target) {
    ByteSize target_length_ = misc_str_strlen(target);
    UInt64   lower_bound_   = 0x6161616161616161ULL; // 'a' = 0x61
    UInt64   upper_bound_   = 0x7A7A7A7A7A7A7A7AULL; // 'z' = 0x7a
    UInt64   clear_mask_    = 0x20;

    // process chunks of 16 bytes
    for (ByteSize i = 0; i < target_length_; i += 16) {
        UInt8    buffer_[16] = {'\0'};
        ByteSize chunk_size_ = (target_length_ - i) < 16 ? (target_length_ - i) : 16;
        hal_mem_memcpy(buffer_, &target[i], chunk_size_);

        // handle the first 8-byte chunk
        UInt64 chunk1_ = 0, chunk2_ = 0;
        hal_mem_memcpy(&chunk1_, buffer_, chunk_size_ >= 8 ? 8 : chunk_size_);
        UInt64 check1_ = _container_string_range_check_batched(chunk1_, lower_bound_, upper_bound_);
        chunk1_ &= ~(check1_ * clear_mask_);
        hal_mem_memcpy(buffer_, &chunk1_, chunk_size_ >= 8 ? 8 : chunk_size_);

        // handle the second 8-byte chunk (if not null)
        if (chunk_size_ > 8) {
            hal_mem_memcpy(&chunk2_, buffer_ + 8, chunk_size_ - 8);
            UInt64 check2_ = _container_string_range_check_batched(chunk2_, lower_bound_, upper_bound_);
            chunk2_ &= ~(check2_ * clear_mask_);
            hal_mem_memcpy(buffer_ + 8, &chunk2_, chunk_size_ - 8);
        }

        // write the transformed chunk back
        hal_mem_memcpy(&target[i], buffer_, chunk_size_);
    }

    // handle remaining scalar bytes
    ByteSize remain_start = target_length_ & ~15;
    for (ByteSize i = remain_start; i < target_length_; ++i) {
        if (target[i] >= 'a' && target[i] <= 'z') {
            target[i] &= ~0x20;
        }
    }

    return target;
}

Bool container_string_lowercase(String str) {
    if (!str || container_string_isempty(str))
        return false;

    StringData *data_ = _container_string_get_internal_data(str);
    data_->_str       = _container_string_lowercase_batched(data_->_str);

    return true;
}

Bool container_string_uppercase(String str) {
    if (!str || container_string_isempty(str))
        return false;

    StringData *data_ = _container_string_get_internal_data(str);
    data_->_str       = _container_string_uppercase_batched(data_->_str);

    return true;
}

#endif

Bool container_string_trim(String str) {
    if (!str || container_string_isempty(str))
        return false;

    StringData *data_ = _container_string_get_internal_data(str);

    // perform removal of leading white-space...
    {
        BytePtr  plead_      = VT_CAST(BytePtr, data_->_str);
        ByteSize lead_count_ = 0;

        while (isspace(VT_CAST(UInt8, *plead_))) {
            ++lead_count_;
            ++plead_;
        }

        hal_mem_memmove(data_->_str, data_->_str + lead_count_, data_->_length - lead_count_);
        data_->_length -= lead_count_;
    }

    // perform remove of trailing white-space...
    {
        BytePtr  ptrail_      = VT_CAST(BytePtr, data_->_str) + data_->_length - 1;
        ByteSize trail_count_ = 0;

        while (isspace(VT_CAST(UInt8, *ptrail_))) {
            ++trail_count_;
            ++ptrail_;
        }

        hal_mem_memzero(ptrail_, trail_count_);
        data_->_length -= trail_count_;
    }

    return true;
}

Str      container_string_get(String str) { return (_container_string_get_internal_data(str)->_str); }
ByteSize container_string_length(String str) { return (_container_string_get_internal_data(str)->_length); }
ByteSize container_string_capacity(String str) { return (_container_string_get_internal_data(str)->_capacity); }
Bool     container_string_isempty(String str) { return (_container_string_get_internal_data(str)->_length == 0); }
Bool     container_string_index_valid(String str, const ByteSize index) {
    return (str && !container_string_isempty(str) && (index >= 0) &&
            (index < VT_CAST(Int32, _container_string_get_internal_data(str)->_length)));
}
Str container_string_mid(String str, const ByteSize start_index) {
    if (!str || container_string_isempty(str) || !container_string_index_valid(str, start_index))
        return NULL;

    StringData *data_ = _container_string_get_internal_data(str);

    if (data_->_length - start_index == 0)
        return NULL;

    return VT_CAST(Str, data_->_str + start_index);
}
