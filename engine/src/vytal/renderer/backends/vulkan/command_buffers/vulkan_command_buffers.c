#include "vulkan_command_buffers.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_compute_command_buffers_construct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_compute_command_buffers_destruct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_graphics_command_buffers_construct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    if (memory_zone_allocate("renderer", sizeof(VkCommandBuffer) * window_->_render_context._graphics_in_flight_fence_count, (VoidPtr *)&window_->_render_context._graphics_cmd_buffers, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_COMMAND_BUFFERS_ALLOCATE_FAILED;

    VkCommandBufferAllocateInfo alloc_info_ = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,

        .commandPool        = window_->_render_context._graphics_cmd_pool,
        .commandBufferCount = window_->_render_context._graphics_in_flight_fence_count,

        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    };

    if (vkAllocateCommandBuffers(context_->_device, &alloc_info_, window_->_render_context._graphics_cmd_buffers) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_COMMAND_BUFFERS_ALLOCATE_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_graphics_command_buffers_destruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    for (ByteSize i = 0; i < window_->_render_context._graphics_in_flight_fence_count; ++i) {
        if (window_->_render_context._graphics_cmd_buffers[i] != VK_NULL_HANDLE)
            vkResetCommandBuffer(window_->_render_context._graphics_cmd_buffers[i], 0);
    }

    vkFreeCommandBuffers(
        context_->_device,
        window_->_render_context._graphics_cmd_pool,
        window_->_render_context._graphics_in_flight_fence_count,
        window_->_render_context._graphics_cmd_buffers);

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_cmd_buffers, sizeof(VkCommandBuffer) * window_->_render_context._graphics_in_flight_fence_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_COMMAND_BUFFERS_DEALLOCATE_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}
