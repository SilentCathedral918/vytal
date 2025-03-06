#pragma once

#include "vytal/defines/core/types.h"

// formats -------------------------------------------------------------- //

typedef enum Mesh_Format {
    MESH_FORMAT_GLTF,
    MESH_FORMAT_GLB,
    MESH_FORMAT_OBJ,
    MESH_FORMAT_FBX
} MeshFormat;

// return codes --------------------------------------------------------- //

typedef enum Mesh_Result {
    MESH_SUCCESS                   = 0,
    MESH_ERROR_INVALID_PARAM       = -1,
    MESH_ERROR_FILE_OPEN_FAILED    = -2,
    MESH_ERROR_FILE_CLOSE_FAILED   = -3,
    MESH_ERROR_FILE_PARSE_FAILED   = -4,
    MESH_ERROR_LOAD_FAILED         = -5,
    MESH_ERROR_UNLOAD_FAILED       = -6,
    MESH_ERROR_ALLOCATION_FAILED   = -7,
    MESH_ERROR_DEALLOCATION_FAILED = -8,
    MESH_ERROR_ALREADY_LOADED      = -9
} MeshResult;

typedef enum Mesh_Module_Result {
    MESH_MODULE_SUCCESS                   = 0,
    MESH_MODULE_ERROR_ALREADY_INITIALIZED = -1,
    MESH_MODULE_ERROR_NOT_INITIALIZED     = -2,
    MESH_MODULE_ERROR_ALLOCATION_FAILED   = -3,
    MESH_MODULE_ERROR_DEALLOCATION_FAILED = -4,
    MESH_MODULE_ERROR_INVALID_PARAM       = -5,
    MESH_MODULE_ERROR_REGISTER_FAILED     = -6,
    MESH_MODULE_ERROR_UNREGISTER_FAILED   = -7
} MeshModuleResult;

// types ---------------------------------------------------------------- //

typedef struct Mesh_Handle {
    ByteSize _vertex_offset;
    UInt32   _vertex_count;
    ByteSize _vertex_size;

    ByteSize _index_offset;
    UInt32   _index_count;
    ByteSize _index_size;

    ByteSize *_texture_offsets;
    UInt32    _texture_count;
    ByteSize *_texture_sizes;

    MeshFormat _format;
    ByteSize   _memory_size;
} *Mesh;
