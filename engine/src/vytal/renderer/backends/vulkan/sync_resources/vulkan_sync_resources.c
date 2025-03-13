#include "vulkan_sync_resources.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/renderer/backends/vulkan/helpers/vulkan_helpers.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_compute_sync_resources_construct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_compute_sync_resources_destruct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_graphics_sync_resources_construct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    VkFenceCreateInfo fence_info_ = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    VkSemaphoreCreateInfo semaphore_info_ = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    window_->_render_context._graphics_in_flight_fence_count = window_->_render_context._swapchain_image_count;

    if (memory_zone_allocate("renderer", sizeof(VkFence) * window_->_render_context._graphics_in_flight_fence_count, (VoidPtr *)&window_->_render_context._graphics_in_flight_fences, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_CONSTRUCT_FAILED;

    if (memory_zone_allocate("renderer", sizeof(VkSemaphore) * window_->_render_context._graphics_in_flight_fence_count, (VoidPtr *)&window_->_render_context._graphics_image_available_semaphores, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_CONSTRUCT_FAILED;

    if (memory_zone_allocate("renderer", sizeof(VkSemaphore) * window_->_render_context._graphics_in_flight_fence_count, (VoidPtr *)&window_->_render_context._graphics_render_complete_semaphores, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_CONSTRUCT_FAILED;

    for (ByteSize i = 0; i < window_->_render_context._graphics_in_flight_fence_count; ++i) {
        // in-flight fence
        if (vkCreateFence(context_->_device, &fence_info_, NULL, &window_->_render_context._graphics_in_flight_fences[i]) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_CONSTRUCT_FAILED;

        // image-ready semaphore
        if (vkCreateSemaphore(context_->_device, &semaphore_info_, NULL, &window_->_render_context._graphics_image_available_semaphores[i]) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_CONSTRUCT_FAILED;

        // render-complete semaphore
        if (vkCreateSemaphore(context_->_device, &semaphore_info_, NULL, &window_->_render_context._graphics_render_complete_semaphores[i]) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_CONSTRUCT_FAILED;
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_graphics_sync_resources_destruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    for (ByteSize i = 0; i < window_->_render_context._graphics_in_flight_fence_count; ++i) {
        // render-complete semaphore
        if (window_->_render_context._graphics_render_complete_semaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(context_->_device, window_->_render_context._graphics_render_complete_semaphores[i], NULL);

        // image-ready semaphore
        if (window_->_render_context._graphics_image_available_semaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(context_->_device, window_->_render_context._graphics_image_available_semaphores[i], NULL);

        // in-flight fence
        if (window_->_render_context._graphics_in_flight_fences[i] != VK_NULL_HANDLE)
            vkDestroyFence(context_->_device, window_->_render_context._graphics_in_flight_fences[i], NULL);
    }

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_render_complete_semaphores, sizeof(VkSemaphore) * window_->_render_context._graphics_in_flight_fence_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_DESTRUCT_FAILED;

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_image_available_semaphores, sizeof(VkSemaphore) * window_->_render_context._graphics_in_flight_fence_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_DESTRUCT_FAILED;

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_in_flight_fences, sizeof(VkFence) * window_->_render_context._graphics_in_flight_fence_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SYNC_RESOURCES_DESTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}
