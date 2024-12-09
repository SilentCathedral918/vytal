#include "application.h"

#include "vytal/core/memory/allocators/arena.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"

#include <stdio.h>
#include <stdlib.h>

void test_mem_manager(void) {
    static MemoryManager manager;
    memory_manager_startup(&manager);

    ArenaAllocator test_allocator = memory_manager_construct_allocator(&manager, "Test", 200, ALLOCTYPE_ARENA);
    allocator_arena_allocate(test_allocator, 180);

    if (!memory_manager_report(&manager, "Test")) {
        misc_console_writeln("no such entry as %s", "Test");
    }

    if (!memory_manager_destruct_allocator(&manager, "Test")) {
        misc_console_writeln("Failed to destruct allocator for %s", "Test");
    }

    memory_manager_report_all(&manager);

    memory_manager_shutdown(&manager);
}