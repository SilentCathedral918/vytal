#include "window.h"

#include "vytal/core/containers/string/string.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/platform/window/backends/glfw/backend_glfw.h"

#define WINDOW_INVALID_POSITION (-1)
#define WINDOW_INVALID_SIZE (-1)
#define WINDOW_INVALID_FRAME (-1.0)
#define WINDOW_ERROR_TITLE "INVALID_WINDOW"
#define BACKEND_ERROR_TITLE "INVALID_BACKEND_API"

typedef struct Window_Backend_API {
    WindowResult (*_startup)(void);
    WindowResult (*_shutdown)(void);
    WindowResult (*_construct)(const WindowProperties, const WindowCallbacks *, Window *);
    WindowResult (*_destruct)(Window);
    WindowResult (*_set_title)(Window, ConstStr);
    WindowResult (*_set_x)(Window, Int32);
    WindowResult (*_set_y)(Window, Int32);
    WindowResult (*_set_position)(Window, Int32, Int32);
    WindowResult (*_set_width)(Window, Int32);
    WindowResult (*_set_height)(Window, Int32);
    WindowResult (*_set_size)(Window, Int32, Int32);
    WindowResult (*_poll_events)(Window);
    WindowResult (*_swap_buffers)(Window);
    VoidPtr (*_get_handle)(Window);
    ConstStr (*_get_title)(Window);
    Int32 (*_get_x)(Window);
    Int32 (*_get_y)(Window);
    Int32 (*_get_width)(Window);
    Int32 (*_get_height)(Window);
    Flt64 (*_get_frame)(Window);
} WindowBackendAPI;

static const WindowBackendAPI glfw_api = {
    window_backend_glfw_startup,
    window_backend_glfw_shutdown,
    window_backend_glfw_construct_handle,
    window_backend_glfw_destruct_handle,
    window_backend_glfw_set_title,
    window_backend_glfw_set_x,
    window_backend_glfw_set_y,
    window_backend_glfw_set_position,
    window_backend_glfw_set_width,
    window_backend_glfw_set_height,
    window_backend_glfw_set_size,
    window_backend_glfw_poll_events,
    window_backend_glfw_swap_buffers,
    window_backend_glfw_get_handle,
    window_backend_glfw_get_title,
    window_backend_glfw_get_x,
    window_backend_glfw_get_y,
    window_backend_glfw_get_width,
    window_backend_glfw_get_height,
    window_backend_glfw_get_frame,
};

static WindowBackendAPI *window_backend_api = NULL;

WindowResult platform_window_startup(const WindowBackend backend) {
    if (window_backend_api) return WINDOW_ERROR_ALREADY_INITIALIZED;

    switch (backend) {
        case WINDOW_BACKEND_GLFW:
            window_backend_api = &glfw_api;
            return window_backend_glfw_startup();

        default:
            return WINDOW_ERROR_INVALID_BACKEND;
    }
}

WindowResult platform_window_shutdown(void) {
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    if (window_backend_api->_shutdown() != WINDOW_SUCCESS)
        return WINDOW_ERROR_DEALLOCATION_FAILED;

    window_backend_api = NULL;
    return WINDOW_SUCCESS;
}

WindowResult platform_window_construct_window(
    const WindowProperties properties,
    const WindowCallbacks *callbacks,
    Window                *out_new_window) {
    if (!callbacks || !properties._width || !properties._height || !out_new_window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_construct(properties, callbacks, out_new_window);
}

WindowResult platform_window_destruct_window(Window window) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_destruct(window);
}

WindowResult platform_window_set_title(Window window, ConstStr title) {
    if (!window || !title) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_set_title(window, title);
}

WindowResult platform_window_set_x(Window window, const Int32 x) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_set_x(window, x);
}

WindowResult platform_window_set_y(Window window, const Int32 y) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_set_y(window, y);
}

WindowResult platform_window_set_position(Window window, const Int32 x, const Int32 y) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_set_position(window, x, y);
}

WindowResult platform_window_set_width(Window window, const Int32 width) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_set_width(window, width);
}

WindowResult platform_window_set_height(Window window, const Int32 height) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_set_height(window, height);
}

WindowResult platform_window_set_size(Window window, const Int32 width, const Int32 height) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_set_size(window, width, height);
}

WindowResult platform_window_poll_events(Window window) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_poll_events(window);
}

WindowResult platform_window_swap_buffers(Window window) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!window_backend_api) return WINDOW_ERROR_NOT_INITIALIZED;

    return window_backend_api->_swap_buffers(window);
}

VoidPtr platform_window_get_handle(Window window) {
    if (!window || !window_backend_api) return NULL;
    return window_backend_api->_get_handle(window);
}

ConstStr platform_window_get_title(Window window) {
    if (!window) return WINDOW_ERROR_TITLE;
    if (!window_backend_api) return BACKEND_ERROR_TITLE;

    return window_backend_api->_get_title(window);
}

Int32 platform_window_get_x(Window window) {
    if (!window || !window_backend_api) return WINDOW_INVALID_POSITION;
    return window_backend_api->_get_x(window);
}

Int32 platform_window_get_y(Window window) {
    if (!window || !window_backend_api) return WINDOW_INVALID_POSITION;
    return window_backend_api->_get_y(window);
}

Int32 platform_window_get_width(Window window) {
    if (!window || !window_backend_api) return WINDOW_INVALID_SIZE;
    return window_backend_api->_get_width(window);
}

Int32 platform_window_get_height(Window window) {
    if (!window || !window_backend_api) return WINDOW_INVALID_SIZE;
    return window_backend_api->_get_height(window);
}

Flt64 platform_window_get_frame(Window window) {
    if (!window || !window_backend_api) return WINDOW_INVALID_FRAME;
    return window_backend_api->_get_frame(window);
}
