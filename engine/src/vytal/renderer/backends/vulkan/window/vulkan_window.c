#include "vulkan_window.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Window_Handle {
    GLFWwindow *_handle;

    VkSurfaceKHR   _surface;
    VkSwapchainKHR _swapchain;
    VkFramebuffer *_frame_buffers;
    VkImageView   *_image_views;
    VkExtent2D     _extent;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_window_construct(VoidPtr instance, VoidPtr *out_window) {
    if (!instance || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    VkInstance instance_ = (VkInstance)instance;
    Window     window_   = *out_window;

    if (glfwCreateWindowSurface(instance_, window_->_handle, NULL, &window_->_surface) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SURFACE_CONSTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_window_destruct(VoidPtr instance, VoidPtr *out_window) {
    if (!out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    VkInstance instance_ = (VkInstance)instance;
    Window     window_   = *out_window;

    vkDestroySurfaceKHR(instance_, window_->_surface, NULL);

    return RENDERER_BACKEND_SUCCESS;
}
