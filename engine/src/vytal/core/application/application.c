#include "application.h"

#include "vytal/core/hash/hash.h"
#include "vytal/core/misc/console/console.h"

void test_hash() {
    ConstStr  test_buf    = "welcome to vytal";
    HashedInt test_hashed = hash_hashstr(test_buf, HASH_MODE_XX64);
    misc_console_writeln("test_buf: %016llx\n", test_hashed);
}