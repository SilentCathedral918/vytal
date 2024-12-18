#pragma once

#include "vytal/defines/core/container.h"
#include "vytal/defines/shared.h"

VT_API String container_string_construct(ConstStr input);
VT_API String container_string_construct_char(const Char c, const ByteSize count);
VT_API String container_string_construct_formatted(ConstStr format, ...);
VT_API Bool   container_string_destruct(String str);
VT_API Bool   container_string_append(String str, ConstStr input);
VT_API Bool   container_string_append_char(String str, const Char c);
VT_API Bool   container_string_append_chars(String str, const Char c, const ByteSize count);
VT_API Bool   container_string_append_formatted(String str, ConstStr format, ...);
VT_API Bool   container_string_filter_char(String str, const Char c);
VT_API Int32  container_string_compare(String left, ConstStr right, Bool sensitive);
VT_API Int32  container_string_compare_string(String left, String right, Bool sensitive);
VT_API Bool   container_string_contains(String str, ConstStr substr, Bool sensitive);
VT_API Bool   container_string_beginswith(String str, ConstStr substr, Bool sensitive);
VT_API Bool   container_string_endswith(String str, ConstStr substr, Bool sensitive);
VT_API Bool   container_string_clear(String str);
VT_API Int32  container_string_search(String str, ConstStr substr, const Bool sensitive);
VT_API Int32  container_string_search_firstchar(String str, const Char c);
VT_API Int32  container_string_search_lastchar(String str, const Char c);
VT_API Bool   container_string_insert_at(String str, ConstStr input, const ByteSize index);
VT_API Bool   container_string_insert_char_at(String str, const Char c, const ByteSize index);
VT_API Bool   container_string_remove_at(String str, const ByteSize index, const ByteSize count);
VT_API Bool   container_string_remove_char_at(String str, const Char c, const ByteSize index);
VT_API Bool   container_string_replace(String str, ConstStr old, ConstStr new, const Bool sensitive);
VT_API Bool   container_string_replace_char(String str, const Char old, const Char new);
VT_API Bool   container_string_lowercase(String str);
VT_API Bool   container_string_uppercase(String str);
VT_API Bool   container_string_trim(String str);

VT_API VT_INLINE Bool container_string_append_int8(String str, Int8 input) {
    return container_string_append_formatted(str, "%hhd", input);
}
VT_API VT_INLINE Bool container_string_append_int16(String str, Int16 input) {
    return container_string_append_formatted(str, "%hd", input);
}
VT_API VT_INLINE Bool container_string_append_int32(String str, Int32 input) {
    return container_string_append_formatted(str, "%d", input);
}
VT_API VT_INLINE Bool container_string_append_int64(String str, Int64 input) {
    return container_string_append_formatted(str, "%lld", input);
}
VT_API VT_INLINE Bool container_string_append_uint8(String str, UInt8 input) {
    return container_string_append_formatted(str, "%hhu", input);
}
VT_API VT_INLINE Bool container_string_append_uint16(String str, UInt16 input) {
    return container_string_append_formatted(str, "%hu", input);
}
VT_API VT_INLINE Bool container_string_append_uint32(String str, UInt32 input) {
    return container_string_append_formatted(str, "%u", input);
}
VT_API VT_INLINE Bool container_string_append_uint64(String str, UInt64 input) {
    return container_string_append_formatted(str, "%llu", input);
}
VT_API VT_INLINE Bool container_string_append_float(String str, Flt32 input) {
    return container_string_append_formatted(str, "%f", input);
}
VT_API VT_INLINE Bool container_string_append_double(String str, Flt64 input) {
    return container_string_append_formatted(str, "%lf", input);
}
VT_API VT_INLINE Bool container_string_append_bool(String str, Bool input) {
    return container_string_append(str, input ? "true" : "false");
}
VT_API VT_INLINE Bool container_string_append_boolnum(String str, Bool input) {
    return container_string_append_char(str, input ? '1' : '0');
}
VT_API VT_INLINE Bool container_string_equals(String left, ConstStr right, Bool sensitive) {
    return (container_string_compare(left, right, sensitive) == 0);
}
VT_API VT_INLINE Bool container_string_equals_string(String left, String right, Bool sensitive) {
    return (container_string_compare_string(left, right, sensitive) == 0);
}

VT_API Str      container_string_get(String str);
VT_API ByteSize container_string_length(String str);
VT_API ByteSize container_string_capacity(String str);
VT_API Bool     container_string_isempty(String str);
VT_API Bool     container_string_index_valid(String str, const ByteSize index);
VT_API Str      container_string_mid(String str, const ByteSize start_index);
