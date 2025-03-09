#include "vulkan_window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/renderer/backends/vulkan/swapchain/vulkan_swapchain.h"

struct Window_Handle {
    GLFWwindow *_handle;

    VkSurfaceKHR _surface;

    VkSwapchainKHR     _curr_swapchain;
    VkSwapchainKHR     _prev_swapchain;
    VkSurfaceFormatKHR _swapchain_surface_format;
    VkPresentModeKHR   _swapchain_present_mode;
    UInt32             _swapchain_image_count;
    VkImage           *_swapchain_images;
    VkImageView       *_swapchain_image_views;
    VkExtent2D         _swapchain_extent;

    VkFramebuffer *_frame_buffers;

    GraphicsPipelineType _active_pipeline;
    UInt32               _frame_index;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_window_construct(VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)*out_window;

    if (glfwCreateWindowSurface(context_->_instance, window_->_handle, NULL, &window_->_surface) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SURFACE_CONSTRUCT_FAILED;

    RendererBackendResult construct_swapchain_ = renderer_backend_vulkan_swapchain_construct(context_, (VoidPtr *)&context_->_first_window);
    if (construct_swapchain_ != RENDERER_BACKEND_SUCCESS)
        return construct_swapchain_;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_window_destruct(VoidPtr context, VoidPtr *out_window) {
    if (!context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)*out_window;

    RendererBackendResult destruct_swapchain_ = renderer_backend_vulkan_swapchain_destruct(context_, out_window);
    if (destruct_swapchain_ != RENDERER_BACKEND_SUCCESS)
        return destruct_swapchain_;

    vkDestroySurfaceKHR(context_->_instance, window_->_surface, NULL);

    return RENDERER_BACKEND_SUCCESS;
}
