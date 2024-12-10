#include "application.h"

#include "vytal/core/containers/string/string.h"
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

    String test_str = container_string_construct("Hello, world! This is a test sentence.");
    memory_manager_report_all();

    misc_console_writeln("string: %s", container_string_get(test_str));
    misc_console_writeln("length: %llu", container_string_length(test_str));
    misc_console_writeln("capacity: %llu", container_string_capacity(test_str));

    if (!container_string_destruct(test_str)) {
        misc_console_writeln("failed to destruct test_str");
    }
    memory_manager_report_all();

    memory_manager_shutdown();
}