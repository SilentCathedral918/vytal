#include "renderer_module.h"

#include <stdlib.h>
#include <string.h>

#include "vytal/core/containers/array/array.h"
#include "vytal/core/helpers/parse/parse.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/platform/filesystem/filesystem.h"
#include "vytal/renderer/backends/renderer_backend.h"

typedef struct Renderer_Module_State {
    RendererBackend _backend;

    Bool     _initialized;
    ByteSize _memory_size;
} RendererModuleState;

static RendererModuleState *state = NULL;

RendererModuleResult renderer_module_startup(File *file) {
    if (!file) return RENDERER_MODULE_ERROR_INVALID_PARAM;
    if (state) return RENDERER_MODULE_ERROR_ALREADY_INITIALIZED;

    ByteSize alloc_size_ = 0;
    if (memory_zone_allocate("modules", sizeof(RendererModuleState), (VoidPtr *)&state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return RENDERER_MODULE_ERROR_ALLOCATION_FAILED;
    memset(state, 0, alloc_size_);

    RendererBackendType backend_type_ = 0;

    ByteSize seek_length_ = 0;
    Str      line_        = calloc(1, LINE_BUFFER_MAX_SIZE);
    while (platform_filesystem_read_line(file, &seek_length_, &line_) == FILE_SUCCESS) {
        Str trimmed_ = line_;

        if (parse_trim_whitespace(&trimmed_) != PARSE_SUCCESS)
            return RENDERER_MODULE_ERROR_PARSE_FAILED;

        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        // check for new section header (e.g. [section_name])
        if (*trimmed_ == '[') {
            platform_filesystem_seek_from_current(file, -seek_length_);
            break;
        }

        Char key_[LINE_BUFFER_MAX_SIZE]   = {0};
        Char value_[LINE_BUFFER_MAX_SIZE] = {0};
        if (!parse_key_value(trimmed_, key_, value_)) continue;

        if (!strcmp(key_, "backend")) {
            if (!strcmp(value_, "vulkan"))
                backend_type_ = RENDERER_BACKEND_VULKAN;
            else
                continue;
        }
    }
    free(line_);

    if (renderer_backend_startup(backend_type_, &state->_backend) != RENDERER_BACKEND_SUCCESS) {
        if (memory_zone_deallocate("modules", state, alloc_size_) != MEMORY_ZONE_SUCCESS)
            return RENDERER_MODULE_ERROR_DEALLOCATION_FAILED;

        return RENDERER_MODULE_ERROR_ALLOCATION_FAILED;
    }

    state->_memory_size = alloc_size_;
    state->_initialized = true;

    return RENDERER_MODULE_SUCCESS;
}

RendererModuleResult renderer_module_shutdown(void) {
    if (!state || !state->_initialized) return RENDERER_MODULE_ERROR_NOT_INITIALIZED;

    if (renderer_backend_shutdown(state->_backend) != RENDERER_BACKEND_SUCCESS)
        return RENDERER_MODULE_ERROR_DEALLOCATION_FAILED;
    state->_backend = NULL;

    if (memory_zone_deallocate("modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
        return RENDERER_MODULE_ERROR_DEALLOCATION_FAILED;
    state = NULL;

    return RENDERER_MODULE_SUCCESS;
}

RendererBackend renderer_module_get_backend(void) {
    if (!state || !state->_initialized) return NULL;
    return state->_backend;
}
