#include <string.h>

#include "vulkan_descriptor_sets.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_descriptor_sets_construct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)*out_window;

    if (memory_zone_allocate("renderer", sizeof(DescriptorSet) * window_->_render_context._graphics_in_flight_fence_count, (VoidPtr *)&window_->_render_context._graphics_desc_sets, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_SETS_CONSTRUCT_FAILED;

    for (ByteSize i = 0; i < window_->_render_context._graphics_in_flight_fence_count; ++i) {
        VkDescriptorSetAllocateInfo alloc_info_ = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,

            .descriptorPool     = window_->_render_context._graphics_desc_pool,
            .descriptorSetCount = 1,
            .pSetLayouts        = &window_->_render_context._graphics_desc_set_layout._handle,
        };

        static Int32 desc_set_id_                                  = 1;
        window_->_render_context._graphics_desc_sets[i]._id        = i;
        window_->_render_context._graphics_desc_sets[i]._ref_count = desc_set_id_++;

        if (vkAllocateDescriptorSets(context_->_device, &alloc_info_, &window_->_render_context._graphics_desc_sets[i]._handle) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_SETS_CONSTRUCT_FAILED;

        VkDescriptorBufferInfo buffer_info_ = {
            .buffer = window_->_render_context._graphics_ubos[i]._handle,
            .offset = 0,
            .range  = window_->_render_context._graphics_ubos[i]._size_bytes,
        };

        VkDescriptorImageInfo image_info_ = {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView   = context_->_default_texture._image_view,
            .sampler     = context_->_default_texture._sampler,
        };

        VkWriteDescriptorSet desc_writes_[] = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,

                .descriptorCount = 1,
                .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,

                .pBufferInfo      = &buffer_info_,
                .pImageInfo       = NULL,
                .pTexelBufferView = NULL,

                .dstSet          = window_->_render_context._graphics_desc_sets[i]._handle,
                .dstBinding      = 0,
                .dstArrayElement = 0,
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,

                .descriptorCount = 1,
                .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,

                .pBufferInfo      = NULL,
                .pImageInfo       = &image_info_,
                .pTexelBufferView = NULL,

                .dstSet          = window_->_render_context._graphics_desc_sets[i]._handle,
                .dstBinding      = 1,
                .dstArrayElement = 0,
            },
        };

        vkUpdateDescriptorSets(context_->_device, VYTAL_ARRAY_SIZE(desc_writes_), desc_writes_, 0, NULL);
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_descriptor_sets_destruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)*out_window;

    for (ByteSize i = 0; i < window_->_render_context._graphics_in_flight_fence_count; ++i) {
        if (window_->_render_context._graphics_desc_sets[i]._handle != VK_NULL_HANDLE)
            vkFreeDescriptorSets(context_->_device, window_->_render_context._graphics_desc_pool, 1, &window_->_render_context._graphics_desc_sets[i]._handle);
    }

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_desc_sets, sizeof(DescriptorSet) * window_->_render_context._graphics_in_flight_fence_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_DESCRIPTOR_SETS_DESTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}
