#pragma once

#include "vytal/defines/core/hash.h"
#include "vytal/defines/shared.h"

VT_API HashedInt hash_hashbuffer(const VoidPtr buffer, const ByteSize size, const HashMode mode);
VT_API HashedInt hash_hashstr(ConstStr str, const HashMode mode);