#include "application.h"

#include <stdio.h>

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/delay/delay.h"
#include "vytal/core/hal/exception/exception.h"
#include "vytal/core/memory/manager/memory_manager.h"
#include "vytal/core/misc/console/console.h"

AppResult _application_core_startup(void) {
    exception_startup();

    if (cvar_startup() != CVAR_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (memory_manager_startup() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (misc_console_startup() != CONSOLE_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    return APP_SUCCESS;
}

AppResult _application_core_shutdown(void) {
    if (misc_console_shutdown() != CONSOLE_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (memory_manager_shutdown() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (cvar_shutdown() != CVAR_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    exception_shutdown();

    return APP_SUCCESS;
}

AppResult application_preconstruct(void) {
    AppResult core_startup_ = _application_core_startup();
    if (core_startup_ != APP_SUCCESS)
        return core_startup_;

    return APP_SUCCESS;
}

typedef struct {
    int  id;
    char name[32];
} TestData;

AppResult application_construct(void) {
    Map             my_map;
    ContainerResult result;

    // 1️⃣ Construct map
    result = container_map_construct(sizeof(TestData), &my_map);
    if (result != CONTAINER_SUCCESS) {
        printf("Map construction failed!\n");
        return 1;
    }

    printf("✅ Map constructed successfully.\n");

    // 2️⃣ Insert elements
    TestData data1 = {1, "Alice"};
    TestData data2 = {2, "Bob"};
    TestData data3 = {3, "Charlie"};

    result = container_map_insert(&my_map, "user1", &data1);
    if (result != CONTAINER_SUCCESS) printf("❌ Failed to insert user1.\n");

    result = container_map_insert(&my_map, "user2", &data2);
    if (result != CONTAINER_SUCCESS) printf("❌ Failed to insert user2.\n");

    result = container_map_insert(&my_map, "user3", &data3);
    if (result != CONTAINER_SUCCESS) printf("❌ Failed to insert user3.\n");

    printf("✅ Inserted 3 users.\n");

    // 3️⃣ Search for a value
    TestData *retrieved_data = NULL;
    result                   = container_map_search(my_map, "user2", (VoidPtr *)&retrieved_data);
    if (result == CONTAINER_SUCCESS) {
        printf("🔎 Found user2: ID=%d, Name=%s\n", retrieved_data->id, retrieved_data->name);
    } else {
        printf("❌ User2 not found.\n");
    }

    // 4️⃣ Check contains function
    if (container_map_contains(my_map, "user3")) {
        printf("✅ user3 exists in map.\n");
    } else {
        printf("❌ user3 does not exist.\n");
    }

    // 5️⃣ Remove a key and verify
    result = container_map_remove(&my_map, "user1");
    if (result == CONTAINER_SUCCESS) {
        printf("🗑️ Removed user1 successfully.\n");
    } else {
        printf("❌ Failed to remove user1.\n");
    }

    // Check if removed
    if (!container_map_contains(my_map, "user1")) {
        printf("✅ user1 is no longer in the map.\n");
    } else {
        printf("❌ user1 still exists.\n");
    }

    // 6️⃣ Update a value
    TestData new_data = {99, "Bobby"};
    result            = container_map_update(&my_map, "user2", &new_data);
    if (result == CONTAINER_SUCCESS) {
        printf("✏️ Updated user2 successfully.\n");
    } else {
        printf("❌ Failed to update user2.\n");
    }

    // Verify update
    retrieved_data = NULL;
    container_map_search(my_map, "user2", (VoidPtr *)&retrieved_data);
    if (retrieved_data) {
        printf("🔎 Updated user2: ID=%d, Name=%s\n", retrieved_data->id, retrieved_data->name);
    }

    // 7️⃣ Test resizing (insert more elements)
    for (int i = 4; i <= 20; i++) {
        TestData temp = {i, "ExtraUser"};
        char     key[16];
        sprintf(key, "user%d", i);
        container_map_insert(&my_map, key, &temp);
    }
    printf("✅ Inserted additional users to test resizing.\n");

    retrieved_data = NULL;
    container_map_search(my_map, "user18", (VoidPtr *)&retrieved_data);
    if (retrieved_data) {
        printf("🔎 Updated user18: ID=%d, Name=%s\n", retrieved_data->id, retrieved_data->name);
    }

    retrieved_data = NULL;
    container_map_search(my_map, "user12", (VoidPtr *)&retrieved_data);
    if (retrieved_data) {
        printf("🔎 Updated user12: ID=%d, Name=%s\n", retrieved_data->id, retrieved_data->name);
    }

    // 8️⃣ Check map status
    printf("📏 Map size: %llu\n", container_map_size(my_map));
    printf("📦 Map capacity: %llu\n", container_map_capacity(my_map));

    // 9️⃣ Destroy map
    result = container_map_destruct(my_map);
    if (result == CONTAINER_SUCCESS) {
        printf("🧹 Map destructed successfully.\n");
    } else {
        printf("❌ Map destruction failed.\n");
    }

    return APP_SUCCESS;
}

AppResult application_update(void) {
    return APP_SUCCESS;
}

AppResult application_destruct(void) {
    AppResult core_shutdown_ = _application_core_shutdown();
    if (core_shutdown_ != APP_SUCCESS)
        return core_shutdown_;

    return APP_SUCCESS;
}
