#include "application.h"

#include "vytal/core/containers/map/map.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/managers/memory/memmgr.h"

#include <string.h>

typedef struct TestData {
    int  id;
    char name[32];
} TestData;

Bool print_map_item(const VoidPtr key, const VoidPtr data, VoidPtr user_data) {
    if (!key || !data)
        return false;

    TestData *item = (TestData *)data;
    misc_console_writeln("Key: %lu, Data: { id: %d, name: %s }", (UIntPtr)key, item->id, item->name);
    return true;
}

void test_container_map(void) {
    memory_manager_startup();

    misc_console_writeln("Testing Container Map...");

    // Define test data
    TestData items[] = {{1, "Alice"}, {2, "Bob"}, {3, "Charlie"}, {4, "Daisy"}};

    // Construct the map with data size of TestData
    ByteSize data_size = sizeof(TestData);
    Map      map       = container_map_construct(data_size);
    if (!map) {
        misc_console_writeln("Failed to construct map.");
        return;
    }

    misc_console_writeln("Map constructed successfully.");

    // Insert items
    for (size_t i = 0; i < sizeof(items) / sizeof(items[0]); i++) {
        if (container_map_insert(map, (VoidPtr)&items[i].id, &items[i]))
            misc_console_writeln("Inserted key %lu.", (UIntPtr)(i + 1));
        else
            misc_console_writeln("Failed to insert key %lu.", (UIntPtr)(i + 1));
    }

    misc_console_writeln("done inserting all keys...");

    // Search for a key
    UIntPtr   search_key = (UIntPtr)(&items[2].id);
    TestData *found_item = (TestData *)container_map_search(map, (VoidPtr)search_key);
    if (found_item)
        misc_console_writeln("Found key %lu: { id: %d, name: %s }", (*(int *)search_key), found_item->id, found_item->name);
    else
        misc_console_writeln("Key %lu not found.", (*(int *)search_key));

    // Update a key's value
    TestData new_data = {99, "Updated Bob"};
    if (container_map_update(map, (VoidPtr)search_key, &new_data))
        misc_console_writeln("Updated key %lu successfully.", search_key);
    else
        misc_console_writeln("Failed to update key %lu.", (*(int *)search_key));

    // Confirm the update
    found_item = (TestData *)container_map_search(map, (VoidPtr)search_key);
    if (found_item)
        misc_console_writeln("After update, key %lu: { id: %d, name: %s }", (*(int *)search_key), found_item->id,
                             found_item->name);

    // Remove a key
    if (container_map_remove(map, (VoidPtr)search_key))
        misc_console_writeln("Removed key %lu successfully.", (*(int *)search_key));
    else
        misc_console_writeln("Failed to remove key %lu.", (*(int *)search_key));

    // Verify removal
    found_item = (TestData *)container_map_search(map, (VoidPtr)search_key);
    misc_console_writeln("After removal, search key %lu: %s", (*(int *)search_key), (found_item ? "Found" : "Not Found"));

    // Destroy the map
    if (container_map_destruct(map))
        misc_console_writeln("Map destructed successfully.");
    else
        misc_console_writeln("Failed to destruct map.");

    misc_console_writeln("Container Map Test Completed.");

    memory_manager_shutdown();
}