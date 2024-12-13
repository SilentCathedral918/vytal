#include "hash.h"

#include "vytal/core/hash/xxhash64/xxhash64.h"
#include "vytal/core/misc/string/vtstr.h"

HashedInt hash_hashbuffer(const VoidPtr buffer, const ByteSize size, const HashMode mode) {
    switch (mode) {
    case HASH_MODE_XX64:
        return hash_xx64_hashbuffer(buffer, size);

    default:
        break;
    }

    return 0;
}

HashedInt hash_hashstr(ConstStr str, const HashMode mode) {
    switch (mode) {
    case HASH_MODE_XX64:
        return hash_xx64_hashbuffer(VT_CAST(VoidPtr, str), misc_str_strlen(str));

    default:
        break;
    }

    return 0;
}
