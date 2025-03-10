#include "vulkan_framebuffers.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_framebuffers_construct(const VoidPtr context, const VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    if (memory_zone_allocate("renderer", sizeof(VkFramebuffer) * window_->_render_context._swapchain_image_count, (VoidPtr *)&window_->_render_context._framebuffers, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_ALLOCATION_FAILED;

    for (ByteSize i = 0; i < window_->_render_context._swapchain_image_count; ++i) {
        VkImageView attachments_[] = {
            window_->_render_context._swapchain_image_views[i],
            window_->_render_context._depth_image_view,
        };

        VkFramebufferCreateInfo framebuf_info_ = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,

            .attachmentCount = VYTAL_ARRAY_SIZE(attachments_),
            .pAttachments    = attachments_,

            .width  = window_->_render_context._swapchain_extent.width,
            .height = window_->_render_context._swapchain_extent.height,

            .renderPass = window_->_render_context._render_pass,
            .layers     = 1,
        };

        if (vkCreateFramebuffer(context_->_device, &framebuf_info_, NULL, &window_->_render_context._framebuffers[i]) != VK_SUCCESS) {
            if (memory_zone_deallocate("renderer", window_->_render_context._framebuffers, sizeof(VkFramebuffer) * window_->_render_context._swapchain_image_count) != MEMORY_ZONE_SUCCESS)
                return RENDERER_BACKEND_ERROR_DEALLOCATION_FAILED;

            return RENDERER_BACKEND_ERROR_VULKAN_FRAMEBUFFERS_CONSTRUCT_FAILED;
        }
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_framebuffers_destruct(const VoidPtr context, const VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    for (size_t i = 0; i < window_->_render_context._swapchain_image_count; ++i) {
        if (window_->_render_context._framebuffers[i] != VK_NULL_HANDLE)
            vkDestroyFramebuffer(context_->_device, window_->_render_context._framebuffers[i], NULL);
    }

    if (memory_zone_deallocate("renderer", window_->_render_context._framebuffers, sizeof(VkFramebuffer) * window_->_render_context._swapchain_image_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_DEALLOCATION_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}
