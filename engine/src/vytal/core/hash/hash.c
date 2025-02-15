#include "hash.h"

#include <string.h>

#include "vytal/core/hash/xxhash64/xxhash64.h"

#define INVALID_HASH 0xffffffffu

HashedInt hash_buffer(const VoidPtr buffer, const ByteSize size, const HashMode mode) {
    if (!buffer || !size) return INVALID_HASH;

    switch (mode) {
        case HASH_MODE_XX64:
            return hash_xx64_buffer(buffer, size);

        default:
            return INVALID_HASH;
    }
}

HashedInt hash_str(ConstStr str, const HashMode mode) {
    if (!str) return INVALID_HASH;

    switch (mode) {
        case HASH_MODE_XX64:
            return hash_xx64_buffer(str, strlen(str));

        default:
            return INVALID_HASH;
    }
}
