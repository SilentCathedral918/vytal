#include "renderer_backend.h"
#include "vytal/renderer/backends/vulkan/backend_vulkan.h"

RendererBackendResult renderer_backend_startup(const RendererBackendType backend_type, Window *out_first_window, ConstStr shaders_filepath, RendererBackend *out_backend) {
    if ((backend_type < 0) || (backend_type > RENDERER_BACKEND_VULKAN)) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    if (out_backend && *out_backend) return RENDERER_BACKEND_ERROR_ALREADY_INITIALIZED;

    switch (backend_type) {
        case RENDERER_BACKEND_VULKAN:
            return renderer_backend_vulkan_startup(out_first_window, shaders_filepath, out_backend);

        default:
            return RENDERER_BACKEND_ERROR_INVALID_BACKEND;
    }
}

RendererBackendResult renderer_backend_shutdown(RendererBackend backend) {
    if (!backend) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;

    switch (backend->_type) {
        case RENDERER_BACKEND_VULKAN:
            return renderer_backend_vulkan_shutdown(backend);

        default:
            return RENDERER_BACKEND_ERROR_INVALID_BACKEND;
    }
}

RendererBackendResult renderer_backend_begin_frame(void) {
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_end_frame(void) {
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_render(void) {
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_register_window(RendererBackend backend, Window *out_window) {
    if (!backend) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;

    switch (backend->_type) {
        case RENDERER_BACKEND_VULKAN:
            return renderer_backend_vulkan_add_window(backend, out_window);

        default:
            return RENDERER_BACKEND_ERROR_INVALID_BACKEND;
    }
}

RendererBackendResult renderer_backend_unregister_window(RendererBackend backend, Window *out_window) {
    if (!backend) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;

    switch (backend->_type) {
        case RENDERER_BACKEND_VULKAN:
            return renderer_backend_vulkan_remove_window(backend, out_window);

        default:
            return RENDERER_BACKEND_ERROR_INVALID_BACKEND;
    }
}
