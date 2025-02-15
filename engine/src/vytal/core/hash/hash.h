#pragma once

#include "vytal/defines/core/hash.h"
#include "vytal/defines/shared.h"

VYTAL_API HashedInt hash_buffer(const VoidPtr buffer, const ByteSize size, const HashMode mode);
VYTAL_API HashedInt hash_str(ConstStr str, const HashMode mode);
