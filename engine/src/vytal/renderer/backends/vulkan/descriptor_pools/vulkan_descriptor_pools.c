#include "vulkan_descriptor_pools.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_descriptor_pools_construct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    // compute descriptor pool
    {
        VkDescriptorPoolSize pool_sizes_[] = {
            {
                .type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = MAX_COMPUTE_DESCRIPTOR_SETS,
            },
            {
                .type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = MAX_COMPUTE_DESCRIPTOR_SETS,
            },
        };

        VkDescriptorPoolCreateInfo pool_info_ = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,

            .poolSizeCount = VYTAL_ARRAY_SIZE(pool_sizes_),
            .pPoolSizes    = pool_sizes_,

            .maxSets = MAX_COMPUTE_DESCRIPTOR_SETS,
            .flags   = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        };

        if (vkCreateDescriptorPool(context_->_device, &pool_info_, NULL, &context_->_compute_desc_pool) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_POOLS_CONSTRUCT_FAILED;
    }

    // graphics descriptor pool, for the first window
    {
        VkDescriptorPoolSize pool_sizes_[] = {
            {
                .type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = context_->_first_window->_render_context._swapchain_image_count,
            },
            {
                .type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = context_->_first_window->_render_context._swapchain_image_count,
            },
        };

        VkDescriptorPoolCreateInfo pool_info_ = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,

            .poolSizeCount = VYTAL_ARRAY_SIZE(pool_sizes_),
            .pPoolSizes    = pool_sizes_,

            .maxSets = context_->_first_window->_render_context._swapchain_image_count,
            .flags   = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        };

        if (vkCreateDescriptorPool(context_->_device, &pool_info_, NULL, &context_->_first_window->_render_context._graphics_desc_pool) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_POOLS_CONSTRUCT_FAILED;
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_descriptor_pools_destruct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    // destruction for descriptor pool of the first window will be handled seperately

    // compute descriptor pool
    if (context_->_compute_desc_pool)
        vkDestroyDescriptorPool(context_->_device, context_->_compute_desc_pool, NULL);

    return RENDERER_BACKEND_SUCCESS;
}
