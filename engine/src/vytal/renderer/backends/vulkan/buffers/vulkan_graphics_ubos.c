#include "vulkan_graphics_ubos.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/renderer/backends/vulkan/helpers/vulkan_helpers.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_graphics_ubos_construct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_     = (RendererBackendVulkanContext *)context;
    Window                        window_      = (Window)(*out_window);
    VkDeviceSize                  buffer_size_ = sizeof(UniformBufferObject);

    if (memory_zone_allocate("renderer", sizeof(RendererBuffer) * window_->_render_context._swapchain_image_count, (VoidPtr *)&window_->_render_context._graphics_ubos, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_UBOS_CONSTRUCT_FAILED;

    if (memory_zone_allocate("renderer", sizeof(VoidPtr) * window_->_render_context._swapchain_image_count, (VoidPtr *)&window_->_render_context._graphics_ubos_mapped, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_UBOS_CONSTRUCT_FAILED;

    for (ByteSize i = 0; i < window_->_render_context._swapchain_image_count; ++i) {
        // construct buffer
        {
            RendererBackendResult construct_buffer_ = renderer_backend_vulkan_helpers_construct_buffer(
                context_,
                buffer_size_,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &window_->_render_context._graphics_ubos[i]);
            if (construct_buffer_ != RENDERER_BACKEND_SUCCESS)
                return construct_buffer_;
        }

        // map the buffer
        if (vkMapMemory(context_->_device,
                        window_->_render_context._graphics_ubos[i]._memory,
                        0,
                        buffer_size_,
                        0,
                        &window_->_render_context._graphics_ubos_mapped[i]) != VK_SUCCESS) {
            return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_UBOS_CONSTRUCT_FAILED;
        }
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_graphics_ubos_destruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    for (ByteSize i = 0; i < window_->_render_context._swapchain_image_count; ++i) {
        if ((VkDeviceMemory)window_->_render_context._graphics_ubos[i]._memory != VK_NULL_HANDLE)
            vkUnmapMemory(context_->_device, window_->_render_context._graphics_ubos[i]._memory);

        RendererBackendResult destruct_buffer_ = renderer_backend_vulkan_helpers_destruct_buffer(context_, &window_->_render_context._graphics_ubos[i]);
        if (destruct_buffer_ != RENDERER_BACKEND_SUCCESS)
            return destruct_buffer_;
    }

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_ubos, sizeof(RendererBuffer) * window_->_render_context._swapchain_image_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_UBOS_DESTRUCT_FAILED;

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_ubos_mapped, sizeof(VoidPtr) * window_->_render_context._swapchain_image_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_UBOS_DESTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}
