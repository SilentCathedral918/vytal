#include "vulkan_depth_resources.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/renderer/backends/vulkan/helpers/vulkan_helpers.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_depth_resources_construct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    VkFormat              depth_format_;
    RendererBackendResult search_depth_format_ = renderer_backend_vulkan_helpers_search_depth_format(context_, &depth_format_);
    if (search_depth_format_ != RENDERER_BACKEND_SUCCESS)
        return search_depth_format_;

    // depth image
    RendererBackendResult construct_image_ = renderer_backend_vulkan_helpers_construct_image(
        context_,
        window_->_render_context._swapchain_extent.width,
        window_->_render_context._swapchain_extent.height,
        1,
        depth_format_,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &window_->_render_context._depth_image,
        &window_->_render_context._depth_image_memory);
    if (construct_image_ != RENDERER_BACKEND_SUCCESS)
        return construct_image_;

    // depth image view
    RendererBackendResult construct_image_view_ = renderer_backend_vulkan_helpers_construct_image_view(
        context_,
        window_->_render_context._depth_image,
        depth_format_,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        1, &window_->_render_context._depth_image_view);
    if (construct_image_view_ != RENDERER_BACKEND_SUCCESS)
        return construct_image_view_;

    // transition image layout
    RendererBackendResult transition_image_layout_ = renderer_backend_vulkan_helpers_transition_image_layout(
        context_,
        window_,
        window_->_render_context._depth_image,
        depth_format_,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        1);
    if (transition_image_layout_ != RENDERER_BACKEND_SUCCESS)
        return transition_image_layout_;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_depth_resources_destruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    if (window_->_render_context._depth_image_view != VK_NULL_HANDLE) {
        vkDestroyImageView(context_->_device, window_->_render_context._depth_image_view, NULL);
        window_->_render_context._depth_image_view = VK_NULL_HANDLE;
    }

    if (window_->_render_context._depth_image_memory != VK_NULL_HANDLE) {
        vkFreeMemory(context_->_device, window_->_render_context._depth_image_memory, NULL);
        window_->_render_context._depth_image_memory = VK_NULL_HANDLE;
    }

    if (window_->_render_context._depth_image != VK_NULL_HANDLE) {
        vkDestroyImage(context_->_device, window_->_render_context._depth_image, NULL);
        window_->_render_context._depth_image = VK_NULL_HANDLE;
    }

    return RENDERER_BACKEND_SUCCESS;
}
