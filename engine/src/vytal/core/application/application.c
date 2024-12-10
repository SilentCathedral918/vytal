#include "application.h"

#include "vytal/core/memory/allocators/arena.h"
#include "vytal/core/misc/console/console.h"

#include <stdio.h>
#include <stdlib.h>

void test_mem_manager(void) {
    memory_manager_startup();

    ArenaAllocator test_allocator = memory_manager_construct_allocator_arena("Test", 200);
    allocator_arena_allocate(test_allocator, 180);

    if (!memory_manager_report("Test")) {
        misc_console_writeln("no such entry as %s", "Test");
    }

    Str test_alloc = memory_manager_allocate(0, MEMORY_TAG_CONTAINERS);
    memory_manager_report_all();

    memory_manager_deallocate(test_alloc, MEMORY_TAG_CONTAINERS);
    memory_manager_report_all();

    memory_manager_shutdown();
}