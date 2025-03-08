#include "engine.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/assets/mesh/module/mesh_module.h"
#include "vytal/core/delegates/multicast/multicast.h"
#include "vytal/core/delegates/unicast/unicast.h"
#include "vytal/core/memory/manager/memory_manager.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/modules/window/window.h"
#include "vytal/renderer/module/renderer_module.h"

typedef struct Engine_State {
    Bool     _initialized;
    ByteSize _memory_size;
} EngineState;

static EngineState *state = NULL;

EngineResult _engine_parse_config(ConstStr config_filepath, Window *out_first_window) {
    File file_ = {0};
    if (platform_filesystem_open_file(&file_, config_filepath, FILE_IO_MODE_READ_WRITE, FILE_MODE_BINARY) != FILE_SUCCESS)
        return ENGINE_ERROR_PRECONSTRUCT_FILE_OPEN_FAILED;

    Char section_[FILENAME_BUFFER_MAX_SIZE] = {0};

    ByteSize seek_length_ = 0;
    Str      line_        = calloc(1, LINE_BUFFER_MAX_SIZE);
    while (platform_filesystem_read_line(&file_, &seek_length_, &line_) == FILE_SUCCESS) {
        Str trimmed_ = line_;

        if (parse_trim_whitespace(&trimmed_) != PARSE_SUCCESS)
            return ENGINE_ERROR_PRECONSTRUCT_PARSE_FAILED;

        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        // check for new section header (e.g. [section_name])
        if (*trimmed_ == '[')
            sscanf(trimmed_, "[%[^]]]", section_);

        // handle sections
        {
            // memory_zones section
            if (!strcmp(section_, "memory_zones")) {
                // handle memory manager startup
                if (memory_manager_startup(&file_) != MEMORY_MANAGER_SUCCESS)
                    return ENGINE_ERROR_PRECONSTRUCT_MEMORY_MANAGER_STARTUP_FAILED;
            }

            // loggers section
            else if (!strcmp(section_, "loggers")) {
                if (console_startup() != CONSOLE_SUCCESS)
                    return ENGINE_ERROR_PRECONSTRUCT_CONSOLE_STARTUP_FAILED;

                // handle logger system startup
                if (logger_startup(&file_) != LOGGER_SUCCESS)
                    return ENGINE_ERROR_PRECONSTRUCT_LOGGER_STARTUP_FAILED;
            }

            // input section
            else if (!strncmp(section_, "input", 5)) {
                platform_filesystem_seek_from_current(&file_, -seek_length_);

                if (input_module_startup(&file_) != INPUT_MODULE_SUCCESS)
                    return ENGINE_ERROR_PRECONSTRUCT_INPUT_MODULE_STARTUP_FAILED;
            }

            // window section
            else if (!strcmp(section_, "window")) {
                if (window_module_startup(&file_) != WINDOW_MODULE_SUCCESS)
                    return ENGINE_ERROR_PRECONSTRUCT_WINDOW_MODULE_STARTUP_FAILED;

                // construct first window
                if (window_module_construct_window(out_first_window) != WINDOW_MODULE_SUCCESS)
                    return ENGINE_ERROR_UPDATE_WINDOW_MODULE_UPDATE_FAILED;
            }

            // renderer section
            else if (!strcmp(section_, "renderer")) {
                if (renderer_module_startup(&file_, out_first_window) != RENDERER_MODULE_SUCCESS)
                    return ENGINE_ERROR_PRECONSTRUCT_RENDERER_MODULE_STARTUP_FAILED;
            }
        }

        Char key_[LINE_BUFFER_MAX_SIZE]   = {0};
        Char value_[LINE_BUFFER_MAX_SIZE] = {0};
        if (!parse_key_value(trimmed_, key_, value_)) continue;
    }

    free(line_);

    if (platform_filesystem_close_file(&file_) != FILE_SUCCESS)
        return ENGINE_ERROR_PRECONSTRUCT_FILE_CLOSE_FAILED;

    return ENGINE_SUCCESS;
}

EngineResult _engine_core_startup(void) {
    // delegate systems
    {
        if (delegate_unicast_startup() != DELEGATE_SUCCESS)
            return ENGINE_ERROR_PRECONSTRUCT_ALLOCATION_FAILED;

        if (delegate_multicast_startup() != DELEGATE_SUCCESS)
            return ENGINE_ERROR_PRECONSTRUCT_ALLOCATION_FAILED;
    }

    // asset modules
    {
        if (mesh_module_startup() != MESH_MODULE_SUCCESS)
            return ENGINE_ERROR_PRECONSTRUCT_ALLOCATION_FAILED;
    }

    // allocate application state and configure its members
    {
        ByteSize         allocated_size_  = 0;
        MemoryZoneResult allocate_engine_ = memory_zone_allocate("core", sizeof(EngineState), (VoidPtr *)&state, &allocated_size_);
        if (allocate_engine_ != MEMORY_ZONE_SUCCESS)
            return ENGINE_ERROR_PRECONSTRUCT_MEMORY_MANAGER_STARTUP_FAILED;
        memset(state, 0, sizeof(EngineState));

        state->_initialized = true;
        state->_memory_size = allocated_size_;
    }

    return ENGINE_SUCCESS;
}

EngineResult _engine_core_shutdown(void) {
    // deallocate application state
    {
        MemoryManagerResult deallocate_engine_ = memory_zone_deallocate("core", state, state->_memory_size);
        if (deallocate_engine_ != MEMORY_MANAGER_SUCCESS)
            return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;

        state = NULL;
    }

    // asset modules
    {
        if (mesh_module_shutdown() != MESH_MODULE_SUCCESS)
            return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;
    }

    // delegate systems
    {
        if (delegate_multicast_shutdown() != DELEGATE_SUCCESS)
            return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;

        if (delegate_unicast_shutdown() != DELEGATE_SUCCESS)
            return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;
    }

    // modules
    {
        if (window_module_shutdown() != WINDOW_MODULE_SUCCESS)
            return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;

        if (renderer_module_shutdown() != RENDERER_MODULE_SUCCESS)
            return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;

        if (input_module_shutdown() != INPUT_MODULE_SUCCESS)
            return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;
    }

    if (console_shutdown() != CONSOLE_SUCCESS)
        return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;

    if (logger_shutdown() != LOGGER_SUCCESS)
        return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;

    if (memory_manager_shutdown() != MEMORY_MANAGER_SUCCESS)
        return ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED;

    return ENGINE_SUCCESS;
}

EngineResult engine_preconstruct(ConstStr config_filepath, Window *out_first_window) {
    if (state) return ENGINE_ERROR_ALREADY_INITIALIZED;
    if (!config_filepath) return ENGINE_ERROR_PRECONSTRUCT_INVALID_PARAM;

    EngineResult parse_config_ = _engine_parse_config(config_filepath, out_first_window);
    if (parse_config_ != ENGINE_SUCCESS)
        return parse_config_;

    EngineResult core_startup_ = _engine_core_startup();
    if (core_startup_ != ENGINE_SUCCESS)
        return core_startup_;

    return ENGINE_SUCCESS;
}

EngineResult engine_construct(void) {
    return ENGINE_SUCCESS;
}

EngineResult engine_update(void) {
    // modules
    {
        if (input_module_update() != INPUT_MODULE_SUCCESS)
            return ENGINE_ERROR_UPDATE_INPUT_MODULE_UPDATE_FAILED;

        if (window_module_update() != WINDOW_MODULE_SUCCESS)
            return ENGINE_ERROR_UPDATE_WINDOW_MODULE_UPDATE_FAILED;
    }

    return ENGINE_SUCCESS;
}

EngineResult engine_destruct(void) {
    if (!state || !state->_initialized) return ENGINE_ERROR_NOT_INITIALIZED;

    EngineResult core_shutdown_ = _engine_core_shutdown();
    if (core_shutdown_ != ENGINE_SUCCESS)
        return core_shutdown_;

    return ENGINE_SUCCESS;
}
