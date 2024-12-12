#include "application.h"

#include "vytal/core/containers/string/string.h"
#include "vytal/core/memory/allocators/arena.h"
#include "vytal/core/misc/console/console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_mem_manager(void) {
    memory_manager_startup();

    // Test 1: container_string_construct
    printf("Testing container_string_construct...\n");
    String str1 = container_string_construct("Hello, World!");
    printf("str1: %s\n", container_string_get(str1)); // Expected: "Hello, World!"

    // Test 2: container_string_construct_char
    String str2 = container_string_construct_char('A', 5);
    printf("str2: %s\n", container_string_get(str2)); // Expected: "AAAAA"

    // Test 3: container_string_append
    container_string_append(str1, " How are you?");
    printf("str1 after append: %s\n", container_string_get(str1)); // Expected: "Hello, World! How are you?"

    // Test 4: container_string_insert_at
    container_string_insert_at(str1, "Beautiful ", 7);
    printf("str1 after insert: %s\n", container_string_get(str1)); // Expected: "Hello, Beautiful World! How are you?"

    // Test 5: container_string_remove_at
    container_string_remove_at(str1, 7, 10);                       // Remove "Beautiful "
    printf("str1 after remove: %s\n", container_string_get(str1)); // Expected: "Hello, World! How are you?"

    // Test 6: container_string_trim
    String str3 = container_string_construct("   Trim me!   ");
    container_string_trim(str3);
    printf("str3 after trim: %s\n", container_string_get(str3)); // Expected: "Trim me!"

    // Test 7: container_string_compare
    String str4 = container_string_construct("hello");
    String str5 = container_string_construct("Hello");
    printf("Comparing str4 and str5 (case-sensitive): %d\n",
           container_string_compare_string(str4, str5, true)); // Expected: Non-zero (not equal)
    printf("Comparing str4 and str5 (case-insensitive): %d\n",
           container_string_compare_string(str4, str5, false)); // Expected: 0 (equal)

    // Test 8: container_string_contains
    String str6 = container_string_construct("Hello, World!");
    printf("Does str6 contain 'World' (case-sensitive)? %d\n",
           container_string_contains(str6, "World", true)); // Expected: 1 (true)
    printf("Does str6 contain 'world' (case-insensitive)? %d\n",
           container_string_contains(str6, "world", false)); // Expected: 1 (true)

    // Test 9: container_string_mid
    String str7 = container_string_construct("Hello, World!");
    Str    mid  = container_string_mid(str7, 7);                  // Get substring starting from index 7
    printf("Substring of str7 starting from index 7: %s\n", mid); // Expected: "World!"

    // Test 10: container_string_lowercase
    String str8 = container_string_construct("HELLO WORLD!");
    container_string_lowercase(str8);
    printf("str8 after lowercase: %s\n", container_string_get(str8)); // Expected: "hello world!"

    // Test 11: container_string_uppercase
    String str9 = container_string_construct("hello world!");
    printf("str9 before uppercase: %s\n", container_string_get(str9)); // Expected: "HELLO WORLD!"
    container_string_uppercase(str9);
    printf("str9 after uppercase: %s\n", container_string_get(str9)); // Expected: "HELLO WORLD!"

    // Test 12: container_string_clear
    String str10 = container_string_construct("This will be cleared");
    container_string_clear(str10);
    printf("str10 after clear: %s\n", container_string_get(str10)); // Expected: Empty string

    // Cleanup (if needed, depending on your implementation)
    container_string_destruct(str1);
    container_string_destruct(str2);
    container_string_destruct(str3);
    container_string_destruct(str4);
    container_string_destruct(str5);
    container_string_destruct(str6);
    container_string_destruct(str7);
    container_string_destruct(str8);
    container_string_destruct(str9);
    container_string_destruct(str10);

    memory_manager_shutdown();
}