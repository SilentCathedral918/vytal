#pragma once

#include "vytal/defines/core/types.h"

// return codes --------------------------------------------------------- //

typedef enum Renderer_Backend_Result {
    RENDERER_BACKEND_SUCCESS                   = 0,
    RENDERER_BACKEND_ERROR_ALREADY_INITIALIZED = -1,
    RENDERER_BACKEND_ERROR_NOT_INITIALIZED     = -2,
    RENDERER_BACKEND_ERROR_INVALID_PARAM       = -3,
    RENDERER_BACKEND_ERROR_INVALID_BACKEND     = -4,
    RENDERER_BACKEND_ERROR_ALLOCATION_FAILED   = -5,
    RENDERER_BACKEND_ERROR_DEALLOCATION_FAILED = -6,

    // Vulkan-specific
    RENDERER_BACKEND_ERROR_VULKAN_EXTS_FETCH_FAILED              = -100,
    RENDERER_BACKEND_ERROR_VULKAN_LAYERS_FETCH_FAILED            = -101,
    RENDERER_BACKEND_ERROR_VULKAN_INSTANCE_CONSTRUCT_FAILED      = -102,
    RENDERER_BACKEND_ERROR_VULKAN_INSTANCE_DESTRUCT_FAILED       = -103,
    RENDERER_BACKEND_ERROR_VULKAN_DEBUG_FUNCS_LOAD_FAILED        = -104,
    RENDERER_BACKEND_ERROR_VULKAN_DEBUG_MSG_CONSTRUCT_FAILED     = -105,
    RENDERER_BACKEND_ERROR_VULKAN_DEBUG_MSG_DESTRUCT_FAILED      = -106,
    RENDERER_BACKEND_ERROR_VULKAN_SURFACE_CONSTRUCT_FAILED       = -107,
    RENDERER_BACKEND_ERROR_VULKAN_SURFACE_DESTRUCT_FAILED        = -108,
    RENDERER_BACKEND_ERROR_VULKAN_GPU_SELECT_FAILED              = -109,
    RENDERER_BACKEND_ERROR_VULKAN_DEVICE_CONSTRUCT_FAILED        = -110,
    RENDERER_BACKEND_ERROR_VULKAN_DEVICE_DESTRUCT_FAILED         = -111,
    RENDERER_BACKEND_ERROR_VULKAN_QUEUE_FAMILIES_SEARCH_FAILED   = -112,
    RENDERER_BACKEND_ERROR_VULKAN_COMMAND_POOL_CONSTRUCT_FAILED  = -113,
    RENDERER_BACKEND_ERROR_VULKAN_COMMAND_POOL_DESTRUCT_FAILED   = -114,
    RENDERER_BACKEND_ERROR_VULKAN_SWAPCHAIN_CONSTRUCT_FAILED     = -115,
    RENDERER_BACKEND_ERROR_VULKAN_SWAPCHAIN_POOL_DESTRUCT_FAILED = -116,
} RendererBackendResult;

typedef enum Renderer_Module_Result {
    RENDERER_MODULE_SUCCESS                   = 0,
    RENDERER_MODULE_ERROR_ALREADY_INITIALIZED = -1,
    RENDERER_MODULE_ERROR_NOT_INITIALIZED     = -2,
    RENDERER_MODULE_ERROR_INVALID_PARAM       = -3,
    RENDERER_MODULE_ERROR_ALLOCATION_FAILED   = -4,
    RENDERER_MODULE_ERROR_DEALLOCATION_FAILED = -5,
    RENDERER_MODULE_ERROR_PARSE_FAILED        = -6,
} RendererModuleResult;

// backend -------------------------------------------------------------- //

typedef enum Renderer_Backend_Type {
    RENDERER_BACKEND_VULKAN
} RendererBackendType;

typedef struct Renderer_Backend {
    RendererBackendType _type;
    VoidPtr             _context;

    RendererBackendResult (*_startup)(void);
    RendererBackendResult (*_shutdown)(void);
    RendererBackendResult (*_begin_frame)(void);
    RendererBackendResult (*_end_frame)(void);
    RendererBackendResult (*_render)(void);

    ByteSize _memory_size;
} *RendererBackend;

// buffer --------------------------------------------------------------- //

typedef enum Renderer_Buffer_Type {
    RENDERER_BUFFER_VERTEX,
    RENDERER_BUFFER_INDEX,
    RENDERER_BUFFER_UNIFORM
} RendererBufferType;

typedef struct Renderer_Buffer {
    VoidPtr            _handle;
    ByteSize           _size_bytes;
    RendererBufferType _type;

    ByteSize _memory_size;
} *RendererBuffer;

// graphics pipeline ---------------------------------------------------- //

typedef enum GraphicsPipelineType {
    GRAPHICS_PIPELINE_TYPE_TEXTURED,
    GRAPHICS_PIPELINE_TYPE_SOLID_COLOUR,
    GRAPHICS_PIPELINE_TYPE_TRANSPARENT,
    GRAPHICS_PIPELINE_TYPE_WIREFRAME,

    NUM_GRAPHICS_PIPELINE
} GraphicsPipelineType;
