#pragma once

#include "vytal/defines/core/ctnr.h"
#include "vytal/defines/shared.h"

VT_API String container_string_construct(ConstStr input);
VT_API Bool   container_string_destruct(String str);

VT_API Str      container_string_get(String str);
VT_API ByteSize container_string_length(String str);
VT_API ByteSize container_string_capacity(String str);
