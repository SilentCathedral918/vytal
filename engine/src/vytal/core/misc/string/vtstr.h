#pragma once

#include "vytal/defines/core/types.h"
#include "vytal/defines/shared.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

VT_API VT_INLINE Str   misc_str_strcat(Str dest, ConstStr src) { return strcat(dest, src); }
VT_API VT_INLINE Str   misc_str_strncat(Str dest, ConstStr src, ByteSize count) { return strncat(dest, src, count); }
VT_API VT_INLINE Str   misc_str_strchr(Str str, Char chr) { return strchr(str, chr); }
VT_API VT_INLINE Str   misc_str_strrchr(Str str, Char chr) { return strrchr(str, chr); }
VT_API VT_INLINE Int32 misc_str_strcmp(ConstStr left, ConstStr right, Bool sensitive) {
#if defined(_MSC_VER)
    return sensitive ? strcmp(left, right) : strcmpi(left, right);
#else
    return sensitive ? strcmp(left, right) : strcasecmp(left, right);
#endif
}
VT_API VT_INLINE Int32 misc_str_strncmp(ConstStr left, ConstStr right, ByteSize count, Bool sensitive) {
#if defined(_MSC_VER)
    return sensitive ? strncmp(left, right, count) : strnicmp(left, right, count);
#else
    return sensitive ? strncmp(left, right, count) : strncasecmp(left, right, count);
#endif
}
VT_API VT_INLINE Str      misc_str_strcpy(Str dest, ConstStr src) { return strcpy(dest, src); }
VT_API VT_INLINE Str      misc_str_strncpy(Str dest, ConstStr src, ByteSize count) { return strncpy(dest, src, count); }
VT_API VT_INLINE ByteSize misc_str_strspn(Str str, ConstStr mask) { return strspn(str, mask); }
VT_API VT_INLINE ByteSize misc_str_strcspn(Str str, ConstStr mask) { return strcspn(str, mask); }
VT_API VT_INLINE Str      misc_str_strdup(ConstStr str) { return strdup(str); }
VT_API VT_INLINE ByteSize misc_str_strlen(ConstStr str) { return strlen(str); }
VT_API VT_INLINE Str      misc_str_strerr(Int32 errorNum) { return strerror(errorNum); }
VT_API VT_INLINE Str      misc_str_strpbrk(Str str, ConstStr mask) { return strpbrk(str, mask); }
VT_API VT_INLINE Str      misc_str_strtok(Str str, ConstStr delim) { return strtok(str, delim); }
VT_API VT_INLINE ByteSize misc_str_strxfrm(Str dest, ConstStr src, ByteSize count) { return strxfrm(dest, src, count); }
VT_API VT_INLINE Flt64    misc_str_atof(ConstStr str) { return atof(str); }
VT_API VT_INLINE Int32    misc_str_atoi(ConstStr str) { return atoi(str); }
VT_API VT_INLINE Int64    misc_str_atol(ConstStr str) { return atol(str); }
VT_API VT_INLINE Int64    misc_str_strtol(ConstStr str, Str *end, Int32 base) { return strtol(str, end, base); }
VT_API VT_INLINE UInt64   misc_str_strtoul(ConstStr str, Str *end, Int32 base) { return strtoul(str, end, base); }
VT_API VT_INLINE Flt32    misc_str_strtof(ConstStr str, Str *end) { return strtof(str, end); }
VT_API ConstStr           misc_str_strstr(Str str, ConstStr substr, Bool sensitive);
VT_API Int32              misc_str_varg(Str target, const ByteSize target_size, ConstStr format, VaList args);
VT_API Int32              misc_str_fmt(Str target, const ByteSize target_size, ConstStr format, ...);
