#include "application.h"

#include "vytal/core/containers/map/map.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/managers/memory/memmgr.h"

#include <string.h>

void test_map() {
    memory_manager_startup();

    // Initialize a map
    Map map = container_map_construct();
    if (!map) {
        misc_console_writeln("Failed to construct the map");
        return;
    }

    // Test insertion
    const char *key1  = "key1";
    const char *data1 = "data1";
    if (!container_map_insert(map, (VoidPtr)key1, (VoidPtr)data1, strlen(data1) + 1)) {
        misc_console_writeln("Failed to insert key1");
        container_map_destruct(map);
        return;
    }

    // Test searching for an existing key
    VoidPtr result = container_map_search(map, (VoidPtr)key1);
    if (result) {
        misc_console_writeln("Found key1: %s", (char *)result);
    } else {
        misc_console_writeln("key1 not found");
    }

    // Test update
    const char *new_data = "new_data1";
    if (!container_map_update(map, (VoidPtr)key1, (VoidPtr)new_data, strlen(new_data) + 1)) {
        misc_console_writeln("Failed to update key1");
        container_map_destruct(map);
        return;
    }

    result = container_map_search(map, (VoidPtr)key1);
    if (result) {
        misc_console_writeln("Updated key1: %s", (char *)result);
    } else {
        misc_console_writeln("key1 not found after update");
    }

    // Test contains function
    if (container_map_contains(map, (VoidPtr)key1)) {
        misc_console_writeln("key1 is in the map");
    } else {
        misc_console_writeln("key1 is NOT in the map");
    }

    // Test removing an entry
    if (!container_map_remove(map, (VoidPtr)key1)) {
        misc_console_writeln("Failed to remove key1");
        container_map_destruct(map);
        return;
    }

    // Check if key is removed
    result = container_map_search(map, (VoidPtr)key1);
    if (result) {
        misc_console_writeln("key1 found after removal");
    } else {
        misc_console_writeln("key1 successfully removed");
    }

    // Test map size functions
    if (container_map_isempty(map)) {
        misc_console_writeln("Map is empty");
    } else {
        misc_console_writeln("Map is NOT empty");
    }

    // Add some more data
    const char *key2  = "key2";
    const char *data2 = "data2";
    container_map_insert(map, (VoidPtr)key2, (VoidPtr)data2, strlen(data2) + 1);

    const char *key3  = "key3";
    const char *data3 = "data3";
    container_map_insert(map, (VoidPtr)key3, (VoidPtr)data3, strlen(data3) + 1);

    misc_console_writeln("Map length: %zu", container_map_length(map));
    misc_console_writeln("Map capacity: %zu", container_map_capacity(map));

    // Test clearing the map
    if (container_map_clear(map)) {
        misc_console_writeln("Map cleared");
    } else {
        misc_console_writeln("Failed to clear the map");
    }

    // Test map after clearing
    if (container_map_isempty(map)) {
        misc_console_writeln("Map is empty after clear");
    } else {
        misc_console_writeln("Map is NOT empty after clear");
    }

    // Cleanup the map
    if (!container_map_destruct(map)) {
        misc_console_writeln("Failed to destruct the map");
        return;
    }

    memory_manager_shutdown();
}