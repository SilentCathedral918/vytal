#include "application.h"

#include "vytal/audio/audio.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/input/input.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/modules/window/window.h"
#include "vytal/core/platform/window/window.h"
#include "vytal/managers/memory/memmgr.h"
#include "vytal/managers/module/modmgr.h"

#define ENGINE_FRAMERATE_MAX 120
#define OUTPUT_BUFFER_MAX_SIZE VT_SIZE_KB_MULT(32) // 32 KB

typedef struct Application_State {
    UInt64             _prev_frame;
    UInt64             _frame_rate;
    Flt32              _delta_time;
    Bool               _active;
    Bool               _suspended;
    Bool               _initialized;
    PlatformWindow     _window;
    WindowTitlebarFlag _window_titlebar_flags;
    Char               _window_title[64];
    Char               _window_updated_title[OUTPUT_BUFFER_MAX_SIZE];
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

    // init state members
    {
        state->_prev_frame  = 0;
        state->_frame_rate  = ENGINE_FRAMERATE_MAX;
        state->_delta_time  = 0.f;
        state->_window      = window_module_get_main();
        state->_active      = true;
        state->_suspended   = false;
        state->_initialized = true;

        misc_str_strcpy(state->_window_title, window_module_get_properties()._title);
        hal_mem_memzero(state->_window_updated_title, OUTPUT_BUFFER_MAX_SIZE);
    }

    _application_report_status("pre_construct state completed, proceeding to construct stage...");
    return true;
}

Bool application_construct(void) {
    if (!state)
        return false;

    // handle startups here...
    {
        audio_load("test", "test.wav");
        audio_load("report", "bugreporter_succeeded.wav");
        audio_load("indoor", "chatter_ambience.wav");
        audio_load("outdoor", "crickets.wav");

        audio_play("indoor", true);
        audio_play("outdoor", true);
        audio_set_volume("outdoor", 0.0f);
    }

    _application_report_status("construct state completed, proceeding to game loop...");
    return true;
}

Bool application_update(void) {
    if (!state)
        return false;

    Flt64       prev_frame_       = platform_window_get_frame(state->_window);
    Flt64       timer_            = prev_frame_;
    Flt64       delta_time_       = 0.0;
    Flt64       accumulated_time_ = 0.0;
    Flt64       curr_frame_       = 0;
    UInt32      frames_           = 0;
    Flt64       framerate_limit_  = 1.0 / (state->_frame_rate == 0 ? ENGINE_FRAMERATE_MAX : state->_frame_rate);
    WindowProps props_            = window_module_get_properties();

    do {
        // measure frame time
        curr_frame_ = platform_window_get_frame(state->_window);
        delta_time_ = curr_frame_ - prev_frame_;
        prev_frame_ = curr_frame_;

        if (delta_time_ > framerate_limit_)
            delta_time_ = framerate_limit_;

        accumulated_time_ += delta_time_;
        state->_delta_time = delta_time_;

        // update at specified frame-rate
        while (accumulated_time_ >= framerate_limit_) {
            // poll events
            if (!platform_window_poll_events(state->_window))
                return false;

            // handle updates here...
            {
                if (hal_input_is_key_pressed(VT_KEYCODE_T))
                    audio_play("test", false);

                if (hal_input_is_key_pressed(VT_KEYCODE_R))
                    audio_play("report", false);

                if (hal_input_is_key_pressed(VT_KEYCODE_F))
                    audio_cross_interpolate_volume("indoor", "outdoor", 1500);
            }

            // update modules
            if (!module_manager_update_modules(delta_time_, accumulated_time_))
                return false;

            accumulated_time_ -= framerate_limit_;
        }

        // render at maximum possible frame
        {
            // swap buffers
            if (!platform_window_swap_buffers(state->_window))
                return false;

            ++frames_;
        }

        // for each passing second...
        // update the FPS display (if specified)
        // reset the frame count
        if (platform_window_get_frame(state->_window) - timer_ > 1.0) {
            ++timer_;

            // render title-bar
            Int32 length_ = misc_str_fmt(state->_window_updated_title, OUTPUT_BUFFER_MAX_SIZE, "%s", state->_window_title);

            if (VT_BITFLAG_IF_SET(props_._titlebar_flags, WINDOW_TITLEBAR_FLAG_FPS))
                length_ += misc_str_fmt(state->_window_updated_title + length_, OUTPUT_BUFFER_MAX_SIZE - length_, " _ FPS: %d",
                                        frames_);

            if (!platform_window_set_title(state->_window, state->_window_updated_title))
                return false;

            hal_mem_memzero(state->_window_updated_title, length_);

            // reset for the next interval
            frames_ = 0;
        }
    } while (state->_active);

    _application_report_status("game loop terminated, proceeding to cleanup...");
    return true;
}

Bool application_destruct(void) {
    if (!state)
        return false;

    // handle shutdowns here...
    {
    }

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
