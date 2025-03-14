#include <string.h>

#include "vulkan_window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/renderer/backends/vulkan/buffers/vulkan_graphics_ubos.h"
#include "vytal/renderer/backends/vulkan/command_buffers/vulkan_command_buffers.h"
#include "vytal/renderer/backends/vulkan/depth_resources/vulkan_depth_resources.h"
#include "vytal/renderer/backends/vulkan/descriptor_sets/vulkan_descriptor_sets.h"
#include "vytal/renderer/backends/vulkan/framebuffers/vulkan_framebuffers.h"
#include "vytal/renderer/backends/vulkan/graphics_pipelines/vulkan_graphics_pipelines.h"
#include "vytal/renderer/backends/vulkan/render_pass/vulkan_render_pass.h"
#include "vytal/renderer/backends/vulkan/swapchain/vulkan_swapchain.h"
#include "vytal/renderer/backends/vulkan/sync_resources/vulkan_sync_resources.h"

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

    // render pass
    RendererBackendResult construct_render_pass_ = renderer_backend_render_pass_construct(context_, out_window);
    if (construct_render_pass_ != RENDERER_BACKEND_SUCCESS)
        return construct_render_pass_;

    // depth resources
    RendererBackendResult construct_depth_resources_ = renderer_backend_vulkan_depth_resources_construct(context_, out_window);
    if (construct_depth_resources_ != RENDERER_BACKEND_SUCCESS)
        return construct_depth_resources_;

    // framebuffers
    RendererBackendResult construct_framebuffers_ = renderer_backend_vulkan_framebuffers_construct(context_, out_window);
    if (construct_framebuffers_ != RENDERER_BACKEND_SUCCESS)
        return construct_framebuffers_;

    // graphics pipelines
    RendererBackendResult construct_graphics_pipelines_ = renderer_backend_vulkan_graphics_pipelines_construct(context_, out_window);
    if (construct_graphics_pipelines_ != RENDERER_BACKEND_SUCCESS)
        return construct_graphics_pipelines_;

    // graphics UBOs
    RendererBackendResult construct_graphics_ubos_ = renderer_backend_vulkan_graphics_ubos_construct(context_, out_window);
    if (construct_graphics_ubos_ != RENDERER_BACKEND_SUCCESS)
        return construct_graphics_ubos_;

    // graphics sync resources
    RendererBackendResult construct_graphics_sync_resources_ = renderer_backend_vulkan_graphics_sync_resources_construct(context_, out_window);
    if (construct_graphics_sync_resources_ != RENDERER_BACKEND_SUCCESS)
        return construct_graphics_sync_resources_;

    // descriptor sets
    RendererBackendResult construct_descriptor_sets_ = renderer_backend_vulkan_descriptor_sets_construct(context_, out_window);
    if (construct_descriptor_sets_ != RENDERER_BACKEND_SUCCESS)
        return construct_descriptor_sets_;

    // command buffers
    RendererBackendResult construct_cmd_buffers_ = renderer_backend_vulkan_graphics_command_buffers_construct(context_, out_window);
    if (construct_cmd_buffers_ != RENDERER_BACKEND_SUCCESS)
        return construct_cmd_buffers_;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_window_destruct(VoidPtr context, VoidPtr *out_window) {
    if (!context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)*out_window;

    // command buffers
    RendererBackendResult destruct_cmd_buffers_ = renderer_backend_vulkan_graphics_command_buffers_destruct(context_, out_window);
    if (destruct_cmd_buffers_ != RENDERER_BACKEND_SUCCESS)
        return destruct_cmd_buffers_;

    // descriptor sets
    RendererBackendResult destruct_descriptor_sets_ = renderer_backend_vulkan_descriptor_sets_destruct(context_, out_window);
    if (destruct_descriptor_sets_ != RENDERER_BACKEND_SUCCESS)
        return destruct_descriptor_sets_;

    // graphics sync resources
    RendererBackendResult destruct_graphics_sync_resources_ = renderer_backend_vulkan_graphics_sync_resources_destruct(context_, out_window);
    if (destruct_graphics_sync_resources_ != RENDERER_BACKEND_SUCCESS)
        return destruct_graphics_sync_resources_;

    // graphics UBOs
    RendererBackendResult destruct_graphics_ubos_ = renderer_backend_vulkan_graphics_ubos_destruct(context_, out_window);
    if (destruct_graphics_ubos_ != RENDERER_BACKEND_SUCCESS)
        return destruct_graphics_ubos_;

    // graphics pipelines
    RendererBackendResult destruct_graphics_pipelines_ = renderer_backend_vulkan_graphics_pipelines_destruct(context_, out_window);
    if (destruct_graphics_pipelines_ != RENDERER_BACKEND_SUCCESS)
        return destruct_graphics_pipelines_;

    // framebuffers
    RendererBackendResult destruct_framebuffers_ = renderer_backend_vulkan_framebuffers_destruct(context_, out_window);
    if (destruct_framebuffers_ != RENDERER_BACKEND_SUCCESS)
        return destruct_framebuffers_;

    // depth resources
    RendererBackendResult destruct_depth_resources_ = renderer_backend_vulkan_depth_resources_destruct(context_, out_window);
    if (destruct_depth_resources_ != RENDERER_BACKEND_SUCCESS)
        return destruct_depth_resources_;

    // render pass
    RendererBackendResult destruct_render_pass_ = renderer_backend_render_pass_destruct(context_, out_window);
    if (destruct_render_pass_ != RENDERER_BACKEND_SUCCESS)
        return destruct_render_pass_;

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
