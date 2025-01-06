#include "modmgr.h"

#include "vytal/audio/module/audio.h"
#include "vytal/core/memory/allocators/arena.h"
#include "vytal/core/misc/assertions/assertions.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/modules/window/window.h"
#include "vytal/managers/memory/memmgr.h"

typedef struct Module_Manager_State {
    VoidPtr _window_module;
    VoidPtr _input_module;
    VoidPtr _audio_module;
} Module_Manager_State;
static Module_Manager_State *state = NULL;

Bool module_manager_startup_modules(void) {
    if (state)
        return false;

    state = memory_manager_allocate(sizeof(Module_Manager_State), MEMORY_TAG_MODULE);

    // modules startup
    {
        // input module
        {
            state->_input_module = memory_manager_allocate(input_module_get_size(), MEMORY_TAG_MODULE);
            VT_ASSERT_MESSAGE(input_module_startup(state->_input_module),
                              "vytal: module manager _ input module startup failed.");
        }

        // window module
        {
            state->_window_module = memory_manager_allocate(window_module_get_size(), MEMORY_TAG_MODULE);
            VT_ASSERT_MESSAGE(window_module_startup(state->_window_module),
                              "vytal: module manager _ window module startup failed.");
        }

        // audio module
        {
            state->_audio_module = memory_manager_allocate(audio_module_get_size(), MEMORY_TAG_MODULE);
            VT_ASSERT_MESSAGE(audio_module_startup(state->_audio_module),
                              "vytal: module manager _ audio module startup failed.")
        }
    }

    return true;
}

Bool module_manager_update_modules(const Flt32 delta_time) {
    if (!state)
        return false;

    // modules update
    {
        // input module
        VT_ASSERT_MESSAGE(input_module_update(), "vytal: module manager _ input module update failed.");

        // window module
        VT_ASSERT_MESSAGE(window_module_update(), "vytal: module manager _ window module update failed.");

        // audio module
        VT_ASSERT_MESSAGE(audio_module_update(delta_time), "vytal: module manager _ audio module update failed.");
    }

    return true;
}

Bool module_manager_shutdown_modules(void) {
    if (!state)
        return false;

    // modules shutdown
    {
        // audio module
        VT_ASSERT_MESSAGE(audio_module_shutdown(), "vytal: module manager _ audio module shutdown failed.");

        // window module
        VT_ASSERT_MESSAGE(window_module_shutdown(), "vytal: module manager _ window module shutdown failed.");

        // input module
        VT_ASSERT_MESSAGE(input_module_shutdown(), "vytal: module manager _ input module shutdown failed.");
    }

    return true;
}
