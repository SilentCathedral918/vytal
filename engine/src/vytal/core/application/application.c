#include "application.h"

#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/input/input.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/modules/window/window.h"
#include "vytal/core/platform/window/window.h"
#include "vytal/managers/memory/memmgr.h"
#include "vytal/managers/module/modmgr.h"

typedef struct Application_State {
    Bool _active;
    Bool _initialized;

} Application_State;

static Application_State *state = NULL;

Bool _application_core_startup(void) {
    // memory manager startup
    memory_manager_startup();

    // console startup
    misc_console_startup();

    // logger startup
    if (!logger_startup())
        return false;

    return true;
}

Bool _application_core_shutdown(void) {
    // logger startup
    if (!logger_shutdown())
        return false;

    // console startup
    misc_console_shutdown();

    // memory manager startup
    memory_manager_shutdown();

    return true;
}

void _application_report_status(ConstStr status) {
    misc_console_setforeground_rgb(101, 191, 104);
    misc_console_writeln("vytal: %s", status);
    misc_console_reset();
}

void _application_on_event(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputEventData data_ = *(VT_CAST(InputEventData *, data));

    switch (data_._event_code) {
    case VT_EVENTCODE_WINDOW_CLOSE:
        VT_LOG_INFO("Engine", "%s", "EVENTCODE_WINDOW_CLOSE invoked _ shutting down...");
        state->_active = false;
        return;

    default:
        break;
    }
}

void _application_on_key(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputKeyEventData *data_ = VT_CAST(InputKeyEventData *, data);

    input_module_process_key_pressed(data_->_key_code, data_->_event_code == VT_EVENTCODE_KEY_PRESSED);

    switch (data_->_event_code) {
    case VT_EVENTCODE_KEY_PRESSED: {
        VT_LOG_INFO("Engine", "key '%c' pressed!", data_->_key_code);
    } break;

    case VT_EVENTCODE_KEY_RELEASED: {
        VT_LOG_INFO("Engine", "key '%c' released!", data_->_key_code);
    } break;

    default:
        break;
    }
}

Bool application_preconstruct(void) {
    if (!_application_core_startup())
        return false;

    // allocate application state
    state = memory_manager_allocate(sizeof(Application_State), MEMORY_TAG_APPLICATION);

    // init state members
    {
        state->_active      = true;
        state->_initialized = true;
    }

    // perform modules startup
    if (!module_manager_startup_modules())
        return false;

    // construct main window
    if (!window_module_construct_main())
        return false;

    // register events
    {
        input_module_register_event(VT_EVENTCODE_WINDOW_CLOSE, _application_on_event);
        input_module_register_event(VT_EVENTCODE_KEY_PRESSED, _application_on_key);
        input_module_register_event(VT_EVENTCODE_KEY_RELEASED, _application_on_key);
    }

    _application_report_status("pre_construct state completed, proceeding to construct stage...");
    return true;
}

Bool application_construct(void) {
    if (!state)
        return false;

    _application_report_status("construct state completed, proceeding to game loop...");
    return true;
}

Bool application_update(void) {
    if (!state)
        return false;

    do {
        if (!module_manager_update_modules())
            return false;

        if (hal_input_is_key_pressed(VT_KEYCODE_F)) {
            window_module_main_toggle_framerate();
        }

    } while (state->_active);

    _application_report_status("game loop terminated, proceeding to cleanup...");
    return true;
}

Bool application_destruct(void) {
    if (!state)
        return false;

    // unregister events
    {
        input_module_unregister_event(VT_EVENTCODE_WINDOW_CLOSE, _application_on_event);
        input_module_register_event(VT_EVENTCODE_KEY_PRESSED, _application_on_key);
        input_module_register_event(VT_EVENTCODE_KEY_RELEASED, _application_on_key);
    }

    if (!window_module_destruct_main())
        return false;

    // perform modules shutdown
    if (!module_manager_shutdown_modules())
        return false;

    // free and set state members to zero
    {
        hal_mem_memzero(state, sizeof(Application_State));
    }

    // free application state
    memory_manager_deallocate(state, MEMORY_TAG_APPLICATION);

    // memory manager shutdown
    if (!_application_core_shutdown())
        return false;

    _application_report_status("cleanup completed, closing application...");
    return true;
}
