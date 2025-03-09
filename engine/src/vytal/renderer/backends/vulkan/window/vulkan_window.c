#include "vulkan_window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/renderer/backends/vulkan/swapchain/vulkan_swapchain.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_window_construct(VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)*out_window;

    // surface
    if (glfwCreateWindowSurface(context_->_instance, window_->_handle, NULL, &window_->_render_context._surface) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SURFACE_CONSTRUCT_FAILED;

    // graphics command pool
    {
        VkCommandPoolCreateInfo cmd_pool_info_ = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = context_->_queue_families._graphics_index,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        };

        if (vkCreateCommandPool(context_->_device, &cmd_pool_info_, NULL, &window_->_render_context._graphics_cmd_pool) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_COMMAND_POOL_CONSTRUCT_FAILED;
    }

    // swapchain
    RendererBackendResult construct_swapchain_ = renderer_backend_vulkan_swapchain_construct(context_, out_window);
    if (construct_swapchain_ != RENDERER_BACKEND_SUCCESS)
        return construct_swapchain_;

    // swapchain image views
    RendererBackendResult construct_swapchain_image_views_ = renderer_backend_vulkan_swapchain_construct_image_views(context_, out_window);
    if (construct_swapchain_image_views_ != RENDERER_BACKEND_SUCCESS)
        return construct_swapchain_image_views_;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_window_destruct(VoidPtr context, VoidPtr *out_window) {
    if (!context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)*out_window;

    // swapchain image views
    RendererBackendResult destruct_swapchain_image_views_ = renderer_backend_vulkan_swapchain_destruct_image_views(context_, out_window);
    if (destruct_swapchain_image_views_ != RENDERER_BACKEND_SUCCESS)
        return destruct_swapchain_image_views_;

    // swapchain
    RendererBackendResult destruct_swapchain_ = renderer_backend_vulkan_swapchain_destruct(context_, out_window);
    if (destruct_swapchain_ != RENDERER_BACKEND_SUCCESS)
        return destruct_swapchain_;

    // graphics command pool
    vkDestroyCommandPool(context_->_device, window_->_render_context._graphics_cmd_pool, NULL);

    // surface
    vkDestroySurfaceKHR(context_->_instance, window_->_render_context._surface, NULL);

    return RENDERER_BACKEND_SUCCESS;
}
