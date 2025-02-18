#pragma once

#include "vytal/defines/core/containers.h"
#include "vytal/defines/shared.h"

VYTAL_API ContainerResult container_string_construct(ConstStr content, String *out_new_str);
VYTAL_API ContainerResult container_string_construct_char(const Char chr, String *out_new_str);
VYTAL_API ContainerResult container_string_construct_chars(const Char chr, const ByteSize count, String *out_new_str);
VYTAL_API ContainerResult container_string_construct_formatted(String *out_new_str, ConstStr format, ...);
VYTAL_API ContainerResult container_string_destruct(String str);

VYTAL_API ContainerResult container_string_append(String *str, ConstStr content);
VYTAL_API ContainerResult container_string_append_char(String *str, const Char chr);
VYTAL_API ContainerResult container_string_append_chars(String *str, const Char chr, const ByteSize count);
VYTAL_API ContainerResult container_string_append_formatted(String *str, ConstStr format, ...);

VYTAL_API ContainerResult container_string_detach(String *str);
VYTAL_API ContainerResult container_string_detach_ranged(String *str, const ByteSize range);

VYTAL_API ContainerResult container_string_filter_char(String *str, const Char chr);

VYTAL_API ContainerResult container_string_equals(String left, ConstStr right, const Bool case_sentitive, Bool *out_result);
VYTAL_API ContainerResult container_string_equals_string(String left, String right, const Bool case_sensitive, Bool *out_result);

VYTAL_API ContainerResult container_string_contains(String str, ConstStr substr, const Bool case_sensitive, Bool *out_result);

VYTAL_API Str      container_string_get(String str);
VYTAL_API ByteSize container_string_size(String str);
VYTAL_API ByteSize container_string_capacity(String str);
