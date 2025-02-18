#include "application.h"

#include <stdio.h>

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/containers/string/string.h"
#include "vytal/core/hal/exception/exception.h"
#include "vytal/core/memory/manager/memory_manager.h"

AppResult application_preconstruct(void) {
    exception_startup();

    if (cvar_startup() != CVAR_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (memory_manager_startup() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    return APP_SUCCESS;
}

AppResult application_construct(void) {
    // String      str     = NULL;
    // const char *content = "Hello, world!";

    // ContainerResult result = container_string_construct(content, &str);
    // if (result == CONTAINER_SUCCESS) {
    //     printf("String created successfully!\n");
    //     printf("Data: %s\n", container_string_get(str));            // Should print "Hello, world!"
    //     printf("Size: %zu\n", container_string_size(str));          // Should print 13
    //     printf("Capacity: %zu\n", container_string_capacity(str));  // Should print the capacity (e.g., 32, depending on alignment)
    // } else {
    //     printf("Failed to create string\n");
    // }

    // ContainerResult append_str_ = container_string_append(&str, "This is a string: John Doe. This is a number: 42");
    // if (append_str_ == CONTAINER_SUCCESS) {
    //     printf("String appended successfully!\n");
    //     printf("Data: %s\n", container_string_get(str));            // Should print "Hello, world!"
    //     printf("Size: %zu\n", container_string_size(str));          // Should print 13
    //     printf("Capacity: %zu\n", container_string_capacity(str));  // Should print the capacity (e.g., 32, depending on alignment)
    // } else {
    //     printf("Failed to append string\n");
    // }

    // String str2 = NULL;
    // if (container_string_append_formatted(&str2, "This is a string: %s. This is a number: %d. This is a floating number: %g. This is a hexadecimal number: 0x%.8x", "John Doe", 42, 123.456f, 42) == CONTAINER_SUCCESS) {
    //     printf("formatted append successfully!\n");
    //     printf("Data: %s\n", container_string_get(str2));
    //     printf("Size: %zu\n", container_string_size(str2));
    //     printf("Capacity: %zu\n", container_string_capacity(str2));
    // }

    // if (container_string_append_chars(&str2, '=', 10) == CONTAINER_SUCCESS) {
    //     printf("chars appended successfully!\n");
    //     printf("Data: %s\n", container_string_get(str2));
    //     printf("Size: %zu\n", container_string_size(str2));
    //     printf("Capacity: %zu\n", container_string_capacity(str2));
    // }

    // if (container_string_append_char(&str2, '!') == CONTAINER_SUCCESS) {
    //     printf("char appended successfully!\n");
    //     printf("Data: %s\n", container_string_get(str2));
    //     printf("Size: %zu\n", container_string_size(str2));
    //     printf("Capacity: %zu\n", container_string_capacity(str2));
    // }

    // if (container_string_append(&str2, "That's it!") == CONTAINER_SUCCESS) {
    //     printf("appended successfully!\n");
    //     printf("Data: %s\n", container_string_get(str2));
    //     printf("Size: %zu\n", container_string_size(str2));
    //     printf("Capacity: %zu\n", container_string_capacity(str2));
    // }

    // if (container_string_append_formatted(&str2, "Jk: last num: %d!", 21) == CONTAINER_SUCCESS) {
    //     printf("formatted appended successfully!\n");
    //     printf("Data: %s\n", container_string_get(str2));
    //     printf("Size: %zu\n", container_string_size(str2));
    //     printf("Capacity: %zu\n", container_string_capacity(str2));
    // }

    // if (container_string_append_chars(&str2, '.', 200) == CONTAINER_SUCCESS) {
    //     printf("chars appended successfully!\n");
    //     printf("Data: %s\n", container_string_get(str2));
    //     printf("Size: %zu\n", container_string_size(str2));
    //     printf("Capacity: %zu\n", container_string_capacity(str2));
    // }

    String str;
    container_string_construct("Hello, world!", &str);
    container_string_filter_char(&str, 'o');
    printf("Filtered: %s\n", container_string_get(str));

    return APP_SUCCESS;
}

AppResult application_update(void) {
    return APP_SUCCESS;
}

AppResult application_destruct(void) {
    if (memory_manager_shutdown() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (cvar_shutdown() != CVAR_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    exception_shutdown();

    return APP_SUCCESS;
}
