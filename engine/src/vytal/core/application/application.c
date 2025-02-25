#include "application.h"

#include <stdio.h>
#include <string.h>

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/containers/string/string.h"
#include "vytal/core/delegates/multicast/multicast.h"
#include "vytal/core/delegates/unicast/unicast.h"
#include "vytal/core/hal/exception/exception.h"
#include "vytal/core/hal/input/input.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/memory/manager/memory_manager.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/modules/window/window.h"
#include "vytal/core/platform/window/window.h"

#define WINDOW_TITLE_UPDATE_RATE 250  // measured in frames

typedef struct Application_State {
    Bool _initialized;
    Bool _active;
    Bool _suspended;

    Flt64  _prev_frame;
    Flt64  _frame_time_ms;
    Flt64  _draw_time_ms;
    Flt32  _delta_time;
    UInt32 _frame_count;

    Window _window;
    Char   _window_base_title[FILENAME_BUFFER_MAX_SIZE];
    Char   _window_title[FILENAME_BUFFER_MAX_SIZE];

    ByteSize _memory_size;
} ApplicationState;

static ApplicationState *app_state  = NULL;
static ConstStr          base_title = "vytal";

static void _application_on_event(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputEventData *data_ = (InputEventData *)data;

    switch (data_->_event_code) {
        case VYTAL_EVENTCODE_WINDOW_CLOSE:
            app_state->_active = false;
            return;

        default:
            break;
    }
}

static void _application_on_key_pressed(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputKeyEventData *data_ = (InputKeyEventData *)data;
    input_module_process_key_pressed(data_->_key_code, data_->_event_code == VYTAL_EVENTCODE_KEY_PRESSED);
}

static void _application_on_mouse_pressed(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseEventData *data_ = (InputMouseEventData *)data;
    input_module_process_mouse_pressed(data_->_mouse_code, data_->_event_code == VYTAL_EVENTCODE_MOUSE_PRESSED);
}

static void _application_on_mouse_moved(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseMoveEventData *data_ = (InputMouseMoveEventData *)data;
    input_module_process_mouse_moved(data_->_x, data_->_y);
}

static void _application_on_mouse_scrolled(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseScrollEventData *data_ = (InputMouseScrollEventData *)data;
    input_module_process_mouse_scrolled(data_->_scroll_value);
}

AppResult _application_core_startup(void) {
    exception_startup();

    if (cvar_startup() != CVAR_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (memory_manager_startup() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (console_startup() != CONSOLE_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (logger_startup() != LOGGER_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    // delegate systems
    {
        if (delegate_unicast_startup() != DELEGATE_SUCCESS)
            return APP_ERROR_PRECONSTRUCT_LOGIC;

        if (delegate_multicast_startup() != DELEGATE_SUCCESS)
            return APP_ERROR_PRECONSTRUCT_LOGIC;
    }

    // modules
    {
        if (input_module_startup() != INPUT_MODULE_SUCCESS)
            return APP_ERROR_PRECONSTRUCT_LOGIC;

        if (window_module_startup() != WINDOW_MODULE_SUCCESS)
            return APP_ERROR_PRECONSTRUCT_LOGIC;
    }

    // window
    if (window_module_construct_main() != WINDOW_MODULE_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    // register events
    {
        input_module_register_event(VYTAL_EVENTCODE_WINDOW_CLOSE, NULL, _application_on_event);
        input_module_register_event(VYTAL_EVENTCODE_KEY_PRESSED, NULL, _application_on_key_pressed);
        input_module_register_event(VYTAL_EVENTCODE_KEY_RELEASED, NULL, _application_on_key_pressed);
        input_module_register_event(VYTAL_EVENTCODE_MOUSE_PRESSED, NULL, _application_on_mouse_pressed);
        input_module_register_event(VYTAL_EVENTCODE_MOUSE_RELEASED, NULL, _application_on_mouse_pressed);
        input_module_register_event(VYTAL_EVENTCODE_MOUSE_MOVED, NULL, _application_on_mouse_moved);
        input_module_register_event(VYTAL_EVENTCODE_MOUSE_SCROLLED, NULL, _application_on_mouse_scrolled);
    }

    // allocate and configure application state
    {
        ByteSize         allocated_size_ = 0;
        MemoryZoneResult allocate_app_   = memory_zone_allocate("Application", sizeof(ApplicationState), (VoidPtr *)&app_state, &allocated_size_);
        if (allocate_app_ != MEMORY_ZONE_SUCCESS)
            return APP_ERROR_PRECONSTRUCT_LOGIC;
        memset(app_state, 0, sizeof(ApplicationState));

        strcpy(app_state->_window_base_title, base_title);
        app_state->_prev_frame  = 0;
        app_state->_frame_count = 0;
        app_state->_delta_time  = 0.f;
        app_state->_window      = window_module_get_main();
        app_state->_active      = true;
        app_state->_suspended   = false;
        app_state->_initialized = true;
        app_state->_memory_size = allocated_size_;
    }

    return APP_SUCCESS;
}

AppResult _application_core_shutdown(void) {
    // deallocate application state
    {
        MemoryZoneResult deallocate_app_ = memory_zone_deallocate("Application", app_state, app_state->_memory_size);
        if (deallocate_app_ != MEMORY_ZONE_SUCCESS)
            return APP_ERROR_PRECONSTRUCT_LOGIC;

        memset(app_state, 0, sizeof(ApplicationState));
        app_state = NULL;
    }

    // unregister events
    {
        input_module_unregister_event(VYTAL_EVENTCODE_WINDOW_CLOSE, _application_on_event);
        input_module_unregister_event(VYTAL_EVENTCODE_KEY_PRESSED, _application_on_key_pressed);
        input_module_unregister_event(VYTAL_EVENTCODE_KEY_RELEASED, _application_on_key_pressed);
        input_module_unregister_event(VYTAL_EVENTCODE_MOUSE_PRESSED, _application_on_mouse_pressed);
        input_module_unregister_event(VYTAL_EVENTCODE_MOUSE_RELEASED, _application_on_mouse_pressed);
        input_module_unregister_event(VYTAL_EVENTCODE_MOUSE_MOVED, _application_on_mouse_moved);
        input_module_unregister_event(VYTAL_EVENTCODE_MOUSE_SCROLLED, _application_on_mouse_scrolled);
    }

    // window
    if (window_module_destruct_main() != WINDOW_MODULE_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    // modules
    {
        if (window_module_shutdown() != WINDOW_MODULE_SUCCESS)
            return APP_ERROR_DESTRUCT_LOGIC;

        if (input_module_shutdown() != INPUT_MODULE_SUCCESS)
            return APP_ERROR_DESTRUCT_LOGIC;
    }

    // delegate systems
    {
        if (delegate_multicast_shutdown() != DELEGATE_SUCCESS)
            return APP_ERROR_DESTRUCT_LOGIC;

        if (delegate_unicast_shutdown() != DELEGATE_SUCCESS)
            return APP_ERROR_DESTRUCT_LOGIC;
    }

    if (logger_shutdown() != LOGGER_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (console_shutdown() != CONSOLE_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (memory_manager_shutdown() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (cvar_shutdown() != CVAR_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    exception_shutdown();

    return APP_SUCCESS;
}

AppResult application_preconstruct(void) {
    AppResult core_startup_ = _application_core_startup();
    if (core_startup_ != APP_SUCCESS)
        return core_startup_;

    return APP_SUCCESS;
}

AppResult application_construct(void) {
    return APP_SUCCESS;
}

AppResult application_update(void) {
    WindowProperties props_ = window_module_get_properties();
    String           title_ = props_._title;

    // keep the window properties title
    sprintf(app_state->_window_title, "/%s", container_string_get(title_));

    app_state->_prev_frame = platform_window_get_frame(app_state->_window);

    do {
        Flt64 frame_start_ = platform_window_get_frame(app_state->_window);

        // modules
        {
            if (input_module_update() != INPUT_MODULE_SUCCESS)
                return APP_ERROR_UPDATE_LOGIC;

            if (window_module_update() != WINDOW_MODULE_SUCCESS)
                return APP_ERROR_UPDATE_LOGIC;
        }

        app_state->_delta_time = platform_window_get_frame(app_state->_window) - frame_start_;

        // render
        {
            Flt64 render_frame_start_ = platform_window_get_frame(app_state->_window);

            // rendering operations go here...

            Flt64 render_frame_end_  = platform_window_get_frame(app_state->_window);
            app_state->_draw_time_ms = (render_frame_end_ - render_frame_start_) * 1000.0;
        }

        {
            if (platform_window_poll_events(app_state->_window) != WINDOW_SUCCESS)
                return APP_ERROR_UPDATE_LOGIC;

            if (platform_window_swap_buffers(app_state->_window) != WINDOW_SUCCESS)
                return APP_ERROR_UPDATE_LOGIC;
        }

        Flt64 frame_end_ = platform_window_get_frame(app_state->_window);

        app_state->_frame_time_ms = (frame_end_ - frame_start_) * 1000.0;

        // report framerate, update window title
        {
            // clear the title string first
            container_string_clear(&title_);

            // base title
            container_string_append(&title_, app_state->_window_base_title);

            // updated title
            if (VYTAL_BITFLAG_IF_SET(props_._title_bar_flags, WINDOW_TITLE_BAR_FLAG_TITLE))
                container_string_append(&title_, app_state->_window_title);

            // frame time
            if (VYTAL_BITFLAG_IF_SET(props_._title_bar_flags, WINDOW_TITLE_BAR_FLAG_FRAME_TIME))
                container_string_append_formatted(&title_, "      frame_time: %.3f ms", app_state->_frame_time_ms);

            // draw time
            if (VYTAL_BITFLAG_IF_SET(props_._title_bar_flags, WINDOW_TITLE_BAR_FLAG_DRAW_TIME))
                container_string_append_formatted(&title_, "      draw_time: %.3f ms", app_state->_draw_time_ms);

            // update title (based on specified rate)
            if (++app_state->_frame_count == WINDOW_TITLE_UPDATE_RATE) {
                platform_window_set_title(app_state->_window, container_string_get(title_));
                app_state->_frame_count = 0;
            }
        }

    } while (app_state->_active);

    return APP_SUCCESS;
}

AppResult application_destruct(void) {
    AppResult core_shutdown_ = _application_core_shutdown();
    if (core_shutdown_ != APP_SUCCESS)
        return core_shutdown_;

    return APP_SUCCESS;
}
