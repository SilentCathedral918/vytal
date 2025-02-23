#include "backend_glfw.h"

#include <GLFW/glfw3.h>

#include "vytal/core/memory/manager/memory_manager.h"

struct Window_Handle {
    GLFWwindow *_handle;
};

static Bool _backend_glfw_initialized = false;

WindowResult window_backend_glfw_startup(void) {
    if (_backend_glfw_initialized)
        return WINDOW_ERROR_ALREADY_INITIALIZED;

    if (glfwInit() != GLFW_TRUE)
        return WINDOW_ERROR_INITILIZATION_FAILED;

    _backend_glfw_initialized = true;
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_shutdown(void) {
    if (!_backend_glfw_initialized)
        return WINDOW_ERROR_NOT_INITIALIZED;

    glfwTerminate();

    _backend_glfw_initialized = false;
    return WINDOW_SUCCESS;
}
