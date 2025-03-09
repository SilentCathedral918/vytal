#include "vulkan_command_pools.h"

RendererBackendResult renderer_backend_vulkan_command_pools_construct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    // compute command pool
    {
        VkCommandPoolCreateInfo cmd_pool_info_ = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = context_->_queue_families._compute_index,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        };

        if (vkCreateCommandPool(context_->_device, &cmd_pool_info_, NULL, &context_->_compute_cmd_pool) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_COMMAND_POOL_CONSTRUCT_FAILED;
    }

    // graphics command pool
    {
        VkCommandPoolCreateInfo cmd_pool_info_ = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = context_->_queue_families._graphics_index,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        };

        if (vkCreateCommandPool(context_->_device, &cmd_pool_info_, NULL, &context_->_graphics_cmd_pool) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_COMMAND_POOL_CONSTRUCT_FAILED;
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_command_pools_destruct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    // graphics command pool
    if (context_->_graphics_cmd_pool != VK_NULL_HANDLE)
        vkDestroyCommandPool(context_->_device, context_->_graphics_cmd_pool, NULL);

    // compute command pool
    if (context_->_graphics_cmd_pool != VK_NULL_HANDLE)
        vkDestroyCommandPool(context_->_device, context_->_compute_cmd_pool, NULL);

    return RENDERER_BACKEND_SUCCESS;
}
