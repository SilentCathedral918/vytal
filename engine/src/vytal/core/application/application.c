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
        state->_active = false;
        return;

    default:
        break;
    }
}

void _application_on_key_pressed(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputKeyEventData *data_ = VT_CAST(InputKeyEventData *, data);
    input_module_process_key_pressed(data_->_key_code, data_->_event_code == VT_EVENTCODE_KEY_PRESSED);
}

void _application_on_mouse_pressed(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseEventData *data_ = VT_CAST(InputMouseEventData *, data);
    input_module_process_mouse_pressed(data_->_mouse_code, data_->_event_code == VT_EVENTCODE_MOUSE_PRESSED);
}

void _application_on_mouse_moved(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseMoveEventData *data_ = VT_CAST(InputMouseMoveEventData *, data);
    input_module_process_mouse_moved(data_->_x, data_->_y);
}

void _application_on_mouse_scrolled(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseScrollEventData *data_ = VT_CAST(InputMouseScrollEventData *, data);
    input_module_process_mouse_scrolled(data_->_scroll_value);
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
        input_module_register_event(VT_EVENTCODE_KEY_PRESSED, _application_on_key_pressed);
        input_module_register_event(VT_EVENTCODE_KEY_RELEASED, _application_on_key_pressed);
        input_module_register_event(VT_EVENTCODE_MOUSE_PRESSED, _application_on_mouse_pressed);
        input_module_register_event(VT_EVENTCODE_MOUSE_RELEASED, _application_on_mouse_pressed);
        input_module_register_event(VT_EVENTCODE_MOUSE_MOVED, _application_on_mouse_moved);
        input_module_register_event(VT_EVENTCODE_MOUSE_SCROLLED, _application_on_mouse_scrolled);
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

        if (hal_input_is_key_down(VT_KEYCODE_TAB)) {
            if (hal_input_is_key_pressed(VT_KEYCODE_F)) {
                VT_LOG_INFO("Engine", "%s", "Combo Tab + F is performed.");
            }
        } else {
            if (hal_input_is_key_pressed(VT_KEYCODE_F)) {
                window_module_main_toggle_framerate();
            }
        }

        if (hal_input_is_mouse_pressed(VT_MOUSECODE_LEFT)) {
            VT_LOG_INFO("Engine", "%s", "left mouse clicked");
        }

        if (hal_input_is_mouse_pressed(VT_MOUSECODE_MIDDLE)) {
            VT_LOG_INFO("Engine", "%s", "middle mouse clicked");
        }

        if (hal_input_is_mouse_pressed(VT_MOUSECODE_RIGHT)) {
            VT_LOG_INFO("Engine", "%s", "right mouse clicked");
        }

        if (hal_input_is_mouse_moved()) {
            VT_LOG_INFO("Engine", "mouse coord: %d, %d", hal_input_get_mouse_x(), hal_input_get_mouse_y());
        }

        if (hal_input_is_mouse_scrolled()) {
            VT_LOG_INFO("Engine", "scroll val: %d", hal_input_get_mouse_scroll_value());
            VT_LOG_INFO("Engine", "scroll val inv: %d", hal_input_get_mouse_scroll_value_inverted());
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
        input_module_unregister_event(VT_EVENTCODE_KEY_PRESSED, _application_on_key_pressed);
        input_module_unregister_event(VT_EVENTCODE_KEY_RELEASED, _application_on_key_pressed);
        input_module_unregister_event(VT_EVENTCODE_MOUSE_PRESSED, _application_on_mouse_pressed);
        input_module_unregister_event(VT_EVENTCODE_MOUSE_RELEASED, _application_on_mouse_pressed);
        input_module_unregister_event(VT_EVENTCODE_MOUSE_MOVED, _application_on_mouse_moved);
        input_module_unregister_event(VT_EVENTCODE_MOUSE_SCROLLED, _application_on_mouse_scrolled);
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
