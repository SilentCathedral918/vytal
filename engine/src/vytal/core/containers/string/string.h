#pragma once

#include "vytal/defines/core/containers.h"
#include "vytal/defines/shared.h"

VYTAL_API ContainerResult container_string_construct(ConstStr content, String *out_new_str);
VYTAL_API ContainerResult container_string_destruct(String str);

VYTAL_API Str      container_string_get(String str);
VYTAL_API ByteSize container_string_size(String str);
VYTAL_API ByteSize container_string_capacity(String str);
