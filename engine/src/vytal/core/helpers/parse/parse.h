#pragma once

#include "vytal/defines/core/helpers.h"
#include "vytal/defines/core/memory.h"
#include "vytal/defines/shared.h"

VYTAL_API ParseResult parse_trim_whitespace(Str *str);
VYTAL_API ParseResult parse_clean_filepath(Str *filepath);

VYTAL_API Bool parse_key_value(ConstStr line, Str key, Str value);

VYTAL_API ByteSize parse_memory_size(Str value);