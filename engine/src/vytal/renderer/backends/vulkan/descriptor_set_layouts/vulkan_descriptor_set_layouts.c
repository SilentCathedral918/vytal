#include "vulkan_descriptor_set_layouts.h"

#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_descriptor_set_layouts_construct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    // compute descriptor set layout
    {
        if (memory_zone_allocate("renderer", sizeof(DescriptorSetLayout) * MAX_COMPUTE_PIPELINES, (VoidPtr *)&context_->_compute_desc_set_layouts, NULL) != MEMORY_ZONE_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_SET_LAYOUTS_CONSTRUCT_FAILED;
        memset(context_->_compute_desc_set_layouts, 0, sizeof(DescriptorSetLayout) * MAX_COMPUTE_PIPELINES);
    }

    // graphics descriptor set layout, for the first window
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

        context_->_first_window->_render_context._graphics_desc_set_layout._id        = 0;
        context_->_first_window->_render_context._graphics_desc_set_layout._ref_count = 1;

        VkDescriptorSetLayoutCreateInfo desc_set_layout_info_ = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,

            .bindingCount = VYTAL_ARRAY_SIZE(bindings_),
            .pBindings    = bindings_,
        };

        if (vkCreateDescriptorSetLayout(context_->_device, &desc_set_layout_info_, NULL, &context_->_first_window->_render_context._graphics_desc_set_layout._handle) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_SET_LAYOUTS_CONSTRUCT_FAILED;
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_descriptor_set_layouts_destruct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    // destruction for descriptor set layout of the first window will be handled seperately

    // compute descriptor set layout
    {
        for (size_t i = 0; i < context_->_compute_desc_set_layout_count; ++i) {
            DescriptorSetLayout *layout_ = &context_->_compute_desc_set_layouts[i];

            if (layout_->_handle == VK_NULL_HANDLE)
                continue;

            vkDestroyDescriptorSetLayout(context_->_device, layout_->_handle, NULL);
            layout_->_handle = VK_NULL_HANDLE;
        }

        if (memory_zone_deallocate("renderer", context_->_compute_desc_set_layouts, sizeof(DescriptorSetLayout) * MAX_COMPUTE_PIPELINES) != MEMORY_ZONE_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_SET_LAYOUTS_DESTRUCT_FAILED;
        context_->_compute_desc_set_layouts = NULL;
    }

    return RENDERER_BACKEND_SUCCESS;
}
