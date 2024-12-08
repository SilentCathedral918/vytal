#include "application.h"

#include "vytal/core/misc/console/console.h"

#include <stdio.h>
#include <stdlib.h>

void test_mem_manager(void) {
    static MemoryManager manager;
    memory_manager_startup(&manager);

    memory_manager_report(&manager);

    memory_manager_shutdown(&manager);
}