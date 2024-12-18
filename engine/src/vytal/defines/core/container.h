#pragma once

#include "types.h"

#define VT_CONTAINER_DEFAULT_CAPACITY 10
#define VT_CONTAINER_RESIZE_FACTOR 2

// ----------------------------- string ----------------------------- //

typedef struct Container_String {
    VoidPtr _internal_data;
} Container_String;
typedef Container_String *String;

// ------------------------------ map ------------------------------ //

typedef struct Container_Map {
    VoidPtr _internal_data;
} Container_Map;
typedef Container_Map *Map;

// ------------------------------ array ------------------------------ //

typedef struct Container_Array {
    VoidPtr _internal_data;
} Container_Array;
typedef Container_Array *Array;
