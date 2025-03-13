#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/backends/backend_vulkan.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_construct_image(
    const VoidPtr               context,
    const UInt32                width,
    const UInt32                height,
    const UInt32                mip_levels,
    const VkFormat              format,
    const VkImageTiling         tiling,
    const VkImageUsageFlags     usage_flags,
    const VkMemoryPropertyFlags properties,
    VkImage                    *out_image,
    VkDeviceMemory             *out_image_memory);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_destruct_image(
    const VoidPtr  context,
    VkImage        image,
    VkDeviceMemory image_memory);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_construct_image_view(
    const VoidPtr            context,
    const VkImage            image,
    const VkFormat           format,
    const VkImageAspectFlags aspect_flags,
    const UInt32             mip_levels,
    VkImageView             *out_image_view);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_destruct_image_view(
    const VoidPtr context,
    VkImageView   image_view);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_construct_graphics_pipeline(
    ConstStr                                 vertex_shader_filepath,
    ConstStr                                 fragment_shader_filepath,
    const VoidPtr                            context,
    const VoidPtr                            window,
    const RendererGraphicsPipelineType       type,
    const UInt32                             binding_count,
    const VkVertexInputBindingDescription   *bindings,
    const UInt32                             attribute_count,
    const VkVertexInputAttributeDescription *attributes,
    VkPipeline                              *out_pipeline,
    VkPipelineLayout                        *out_pipeline_layout);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_destruct_graphics_pipeline(
    const VoidPtr    context,
    VkPipeline       pipeline,
    VkPipelineLayout pipeline_layout);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_construct_buffer(
    const VoidPtr               context,
    const VkDeviceSize          size,
    const VkBufferUsageFlags    usage_flags,
    const VkMemoryPropertyFlags properties,
    RendererBuffer             *out_new_buffer);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_destruct_buffer(
    const VoidPtr   context,
    RendererBuffer *buffer);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_construct_texture(
    const VoidPtr    context,
    const Window     window,
    ConstStr         texture_filepath,
    RendererTexture *out_new_texture);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_destruct_texture(
    const VoidPtr    context,
    RendererTexture *texture);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_transition_image_layout(
    const VoidPtr            context,
    const VoidPtr            window,
    const VkImage            image,
    const VkFormat           format,
    const VkImageLayout      old_layout,
    const VkImageLayout      new_layout,
    const VkImageAspectFlags aspect_flags,
    const UInt32             mip_levels);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_search_supported_format(
    const VoidPtr              context,
    const VkFormat            *candidates,
    const size_t               candidate_count,
    const VkImageTiling        tiling,
    const VkFormatFeatureFlags features,
    VkFormat                  *out_format);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_search_depth_format(
    const VoidPtr context,
    VkFormat     *out_format);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_search_memory_type(
    const VkPhysicalDevice      gpu,
    const UInt32                type_filter,
    const VkMemoryPropertyFlags properties,
    ByteSize                   *out_memory_type);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_begin_single_time_commands(
    const VoidPtr              context,
    const VkCommandPool       *pool,
    const UInt32               cmd_buffer_count,
    const VkCommandBufferLevel level,
    VkCommandBuffer          **out_buffers);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_end_single_time_commands(
    const VoidPtr        context,
    const VkCommandPool *pool,
    const VkQueue        queue,
    const UInt32         cmd_buffer_count,
    VkCommandBuffer     *buffers);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_read_shader_file(
    ConstStr  filepath,
    ByteSize *out_shader_size,
    UInt32  **out_shader_code);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_construct_default_texture(UInt32 **out_default_texture);

VYTAL_API RendererBackendResult renderer_backend_vulkan_helpers_generate_mipmap(
    const VoidPtr   context,
    const Int32     image_width,
    const Int32     image_height,
    const UInt32    mip_levels,
    const VkFormat  format,
    const VkImage   image,
    VkCommandBuffer cmd_buffer);
