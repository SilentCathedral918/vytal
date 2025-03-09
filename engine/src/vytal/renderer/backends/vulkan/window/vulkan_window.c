#include "vulkan_window.h"

#include <string.h>

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

    // graphics descriptor pool
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

        if (vkCreateDescriptorPool(context_->_device, &pool_info_, NULL, &window_->_render_context._graphics_desc_pool) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_POOLS_CONSTRUCT_FAILED;
    }

    // graphics descriptor set layout
    {
        VkDescriptorSetLayoutBinding bindings_[] = {
            {
                // matrices
                .binding         = 0,
                .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT,
            },
            {
                // albedo texture
                .binding         = 1,
                .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                // normal map
                .binding         = 2,
                .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                // roughness map
                .binding         = 3,
                .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                // ambient occlusion map
                .binding         = 4,
                .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT,
            },
        };

        static UInt32 id_                                             = 1;
        window_->_render_context._graphics_desc_set_layout._id        = id_++;
        window_->_render_context._graphics_desc_set_layout._ref_count = 1;

        VkDescriptorSetLayoutCreateInfo desc_set_layout_info_ = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,

            .bindingCount = VYTAL_ARRAY_SIZE(bindings_),
            .pBindings    = bindings_,
        };

        if (vkCreateDescriptorSetLayout(context_->_device, &desc_set_layout_info_, NULL, &window_->_render_context._graphics_desc_set_layout._handle) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_SET_LAYOUTS_CONSTRUCT_FAILED;
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_window_destruct(VoidPtr context, VoidPtr *out_window) {
    if (!context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)*out_window;

    // graphics descriptor set layout
    if (window_->_render_context._graphics_desc_set_layout._handle != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(context_->_device, window_->_render_context._graphics_desc_set_layout._handle, NULL);
        memset(&window_->_render_context._graphics_desc_set_layout, 0, sizeof(DescriptorSetLayout));
    }

    // graphics descriptor pool
    if (window_->_render_context._graphics_desc_pool)
        vkDestroyDescriptorPool(context_->_device, window_->_render_context._graphics_desc_pool, NULL);

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
