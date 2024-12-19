#include "window.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/managers/memory/memmgr.h"

#include "vytal/core/platform/window/backend/glfw/glfw_window.h"

static WindowBackend window_backend = -1;
static Bool          is_valid_backend() { return window_backend != -1; }

Bool platform_window_startup(const WindowBackend backend) {
    if (is_valid_backend())
        return false;

    switch (backend) {
    case WINDOW_BACKEND_GLFW:
        window_backend = WINDOW_BACKEND_GLFW;
        return glfw_window_startup();

    default:
        return false;
    }
}

void platform_window_shutdown(void) {
    if (!is_valid_backend())
        return;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        glfw_window_shutdown();
        break;

    default:
        break;
    }

    window_backend = -1;
}

PlatformWindow platform_window_construct(const WindowProps properties, const WindowCallbacks *callbacks) {
    if (!is_valid_backend() || !callbacks)
        return NULL;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_construct(properties, callbacks);

    default:
        return NULL;
    }
}

Bool platform_window_destruct(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_destruct(window);

    default:
        return false;
    }
}

VoidPtr platform_window_get_native(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return NULL;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_get_handle(window);

    default:
        return NULL;
    }
}

ConstStr platform_window_get_title(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return "";

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_get_title(window);

    default:
        return "";
    }
}

Int32 platform_window_get_x(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return 0;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_get_x(window);

    default:
        return 0;
    }
}

Int32 platform_window_get_y(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return 0;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_get_y(window);

    default:
        return 0;
    }
}

Int32 platform_window_get_width(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return 0;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_get_width(window);

    default:
        return 0;
    }
}

Int32 platform_window_get_height(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return 0;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_get_height(window);

    default:
        return 0;
    }
}

Bool platform_window_set_title(PlatformWindow window, ConstStr title) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_set_title(window, title);

    default:
        return false;
    }
}

Bool platform_window_set_x(PlatformWindow window, const Int32 x) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_set_x(window, x);

    default:
        return false;
    }
}

Bool platform_window_set_y(PlatformWindow window, const Int32 y) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_set_y(window, y);

    default:
        return false;
    }
}

Bool platform_window_set_position(PlatformWindow window, const Int32 x, const Int32 y) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_set_position(window, x, y);

    default:
        return false;
    }
}

Bool platform_window_set_width(PlatformWindow window, const Int32 width) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_set_width(window, width);

    default:
        return false;
    }
}

Bool platform_window_set_height(PlatformWindow window, const Int32 height) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_set_height(window, height);

    default:
        return false;
    }
}

Bool platform_window_set_size(PlatformWindow window, const Int32 width, const Int32 height) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_set_size(window, width, height);

    default:
        return false;
    }
}

Bool platform_window_poll_events(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_poll_events(window);

    default:
        return false;
    }
}

Bool platform_window_swap_buffers(PlatformWindow window) {
    if (!window || !is_valid_backend())
        return false;

    switch (window_backend) {
    case WINDOW_BACKEND_GLFW:
        return glfw_window_swap_buffers(window);

    default:
        return false;
    }
}
