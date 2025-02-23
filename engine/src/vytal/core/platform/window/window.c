#include "window.h"

#include "vytal/core/containers/string/string.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/platform/window/backends/glfw/backend_glfw.h"

static WindowBackend _window_backend = -1;
Bool                 _window_backend_initialized() { return _window_backend != -1; }

WindowResult window_startup(const WindowBackend backend) {
    if (_window_backend_initialized())
        return WINDOW_ERROR_ALREADY_INITIALIZED;

    switch (backend) {
        case WINDOW_BACKEND_GLFW:
            _window_backend = WINDOW_BACKEND_GLFW;
            return window_backend_glfw_startup();

        default:
            return WINDOW_ERROR_INVALID_BACKEND;
    }
}

WindowResult window_shutdown(void) {
    if (!_window_backend_initialized())
        return WINDOW_ERROR_NOT_INITIALIZED;

    switch (_window_backend) {
        case WINDOW_BACKEND_GLFW:
            WindowResult backend_shutdown_ = window_backend_glfw_shutdown();
            if (backend_shutdown_ != WINDOW_SUCCESS)
                return backend_shutdown_;
            break;

        default:
            return WINDOW_ERROR_INVALID_BACKEND;
    }

    _window_backend = -1;
    return WINDOW_SUCCESS;
}
