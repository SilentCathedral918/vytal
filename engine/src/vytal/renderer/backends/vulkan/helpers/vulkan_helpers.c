#include <stdlib.h>

#include "vulkan_helpers.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/platform/filesystem/filesystem.h"
#include "vytal/renderer/backends/vulkan/helpers/vulkan_helpers.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_helpers_construct_image(
    const VoidPtr               context,
    const UInt32                width,
    const UInt32                height,
    const UInt32                mip_levels,
    const VkFormat              format,
    const VkImageTiling         tiling,
    const VkImageUsageFlags     usage_flags,
    const VkMemoryPropertyFlags properties,
    VkImage                    *out_image,
    VkDeviceMemory             *out_image_memory) {
    if (!context || !width || !height || !mip_levels || !out_image || !out_image_memory) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    VkImageCreateInfo image_info_ = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,

        .imageType     = VK_IMAGE_TYPE_2D,
        .arrayLayers   = 1,
        .format        = format,
        .tiling        = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage         = usage_flags,
        .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
        .samples       = VK_SAMPLE_COUNT_1_BIT,
        .flags         = 0,

        .extent = {
            .width  = width,
            .height = height,
            .depth  = 1,
        },

        .mipLevels = mip_levels,
    };

    if (vkCreateImage(context_->_device, &image_info_, NULL, out_image) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_IMAGE_CONSTRUCT_FAILED;

    VkMemoryRequirements mem_requirements_;
    vkGetImageMemoryRequirements(context_->_device, *out_image, &mem_requirements_);

    ByteSize              memory_type_;
    RendererBackendResult search_memory_type_ = renderer_backend_vulkan_helpers_search_memory_type(
        context_->_gpu,
        mem_requirements_.memoryTypeBits,
        properties,
        &memory_type_);
    if (search_memory_type_ != RENDERER_BACKEND_SUCCESS)
        return search_memory_type_;

    VkMemoryAllocateInfo alloc_info_ = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize  = mem_requirements_.size,
        .memoryTypeIndex = memory_type_,
    };

    if (vkAllocateMemory(context_->_device, &alloc_info_, NULL, out_image_memory) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_IMAGE_CONSTRUCT_FAILED;

    if (vkBindImageMemory(context_->_device, *out_image, *out_image_memory, 0) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_IMAGE_CONSTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_destruct_image(
    const VoidPtr  context,
    VkImage        image,
    VkDeviceMemory image_memory) {
    if (!context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    if (image_memory != VK_NULL_HANDLE)
        vkFreeMemory(context_->_device, image_memory, NULL);

    if (image != VK_NULL_HANDLE)
        vkDestroyImage(context_->_device, image, NULL);

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_construct_image_view(
    const VoidPtr            context,
    const VkImage            image,
    const VkFormat           format,
    const VkImageAspectFlags aspect_flags,
    const UInt32             mip_levels,
    VkImageView             *out_image_view) {
    if (!context || !out_image_view) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    VkImageViewCreateInfo image_view_info_ = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,

        .image = image,

        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = format,

        .subresourceRange = {
            .aspectMask = aspect_flags,

            .baseMipLevel = 0,
            .levelCount   = mip_levels,

            .baseArrayLayer = 0,
            .layerCount     = 1,
        },
    };

    if (vkCreateImageView(context_->_device, &image_view_info_, NULL, out_image_view) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_IMAGE_VIEW_CONSTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_destruct_image_view(
    const VoidPtr context,
    VkImageView   image_view) {
    if (!context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    if (image_view != VK_NULL_HANDLE)
        vkDestroyImageView(context_->_device, image_view, NULL);

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_construct_shader_module(
    const VoidPtr   context,
    ConstStr        shader_filepath,
    VkShaderModule *out_shader_module) {
    if (!context || !shader_filepath || !out_shader_module) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    ByteSize shader_code_size_ = 0;
    UInt32  *shader_code_      = NULL;

    RendererBackendResult read_shader_file_ = renderer_backend_vulkan_helpers_read_shader_file(shader_filepath, &shader_code_size_, &shader_code_);
    if (read_shader_file_ != RENDERER_BACKEND_SUCCESS)
        return read_shader_file_;

    VkShaderModuleCreateInfo module_info_ = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,

        .codeSize = shader_code_size_,
        .pCode    = shader_code_,
    };

    if (vkCreateShaderModule(context_->_device, &module_info_, NULL, out_shader_module) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SHADER_MODULE_CONSTRUCT_FAILED;

    free(shader_code_);
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_destruct_shader_module(
    const VoidPtr  context,
    VkShaderModule shader_module) {
    if (!context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    if (shader_module != VK_NULL_HANDLE)
        vkDestroyShaderModule(context_->_device, shader_module, NULL);

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_construct_graphics_pipeline(
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
    VkPipelineLayout                        *out_pipeline_layout) {
    if (!vertex_shader_filepath ||
        !fragment_shader_filepath ||
        !context ||
        !binding_count ||
        !bindings ||
        !attribute_count ||
        !attributes ||
        !out_pipeline ||
        !out_pipeline_layout) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)window;
    VkShaderModule                vert_shader_, frag_shader_;

    RendererBackendResult construct_vert_shader_module_ = renderer_backend_vulkan_helpers_construct_shader_module(context_, vertex_shader_filepath, &vert_shader_);
    if (construct_vert_shader_module_ != RENDERER_BACKEND_SUCCESS)
        return construct_vert_shader_module_;

    RendererBackendResult construct_frag_shader_module_ = renderer_backend_vulkan_helpers_construct_shader_module(context_, fragment_shader_filepath, &frag_shader_);
    if (construct_frag_shader_module_ != RENDERER_BACKEND_SUCCESS)
        return construct_frag_shader_module_;

    VkPolygonMode   raster_polygon_mode_;
    VkCullModeFlags raster_cull_mode_;
    VkBool32        color_blend_enabled_;
    VkBlendFactor   src_color_blend_factor_;
    VkBlendFactor   dst_color_blend_factor_;
    VkBool32        depth_write_enabled_;

    switch (type) {
        case RENDERER_GRAPHICS_PIPELINE_TYPE_SOLID_COLOR:
            raster_polygon_mode_ = VK_POLYGON_MODE_FILL;
            raster_cull_mode_    = VK_CULL_MODE_BACK_BIT;

            color_blend_enabled_    = VK_FALSE;
            src_color_blend_factor_ = VK_BLEND_FACTOR_SRC_ALPHA;
            dst_color_blend_factor_ = VK_BLEND_FACTOR_SRC_ALPHA;

            depth_write_enabled_ = VK_TRUE;

            break;

        case RENDERER_GRAPHICS_PIPELINE_TYPE_TEXTURED:
            raster_polygon_mode_ = VK_POLYGON_MODE_FILL;
            raster_cull_mode_    = VK_CULL_MODE_BACK_BIT;

            color_blend_enabled_    = VK_FALSE;
            src_color_blend_factor_ = VK_BLEND_FACTOR_ONE;
            dst_color_blend_factor_ = VK_BLEND_FACTOR_ZERO;

            depth_write_enabled_ = VK_TRUE;

            break;

        case RENDERER_GRAPHICS_PIPELINE_TYPE_TRANSPARENT:
            raster_polygon_mode_ = VK_POLYGON_MODE_FILL;
            raster_cull_mode_    = VK_CULL_MODE_NONE;

            color_blend_enabled_    = VK_TRUE;
            src_color_blend_factor_ = VK_BLEND_FACTOR_ONE;
            dst_color_blend_factor_ = VK_BLEND_FACTOR_ZERO;

            depth_write_enabled_ = VK_FALSE;

            break;

        case RENDERER_GRAPHICS_PIPELINE_TYPE_WIREFRAME:
            raster_polygon_mode_ = VK_POLYGON_MODE_LINE;
            raster_cull_mode_    = VK_CULL_MODE_NONE;

            color_blend_enabled_    = VK_FALSE;
            src_color_blend_factor_ = VK_BLEND_FACTOR_ONE;
            dst_color_blend_factor_ = VK_BLEND_FACTOR_ZERO;

            depth_write_enabled_ = VK_TRUE;

            break;

        default:
            return RENDERER_BACKEND_ERROR_VULKAN_INVALID_GRAPHICS_PIPELINE_TYPE;
    }

    VkPipelineShaderStageCreateInfo vert_shader_stage_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,

        .stage  = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert_shader_,
        .pName  = "main",
    };

    VkPipelineShaderStageCreateInfo frag_shader_stage_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,

        .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = frag_shader_,
        .pName  = "main",
    };

    VkPipelineShaderStageCreateInfo shader_stage_infos_[] = {
        vert_shader_stage_info_,
        frag_shader_stage_info_,
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

        .vertexBindingDescriptionCount = binding_count,
        .pVertexBindingDescriptions    = bindings,

        .vertexAttributeDescriptionCount = attribute_count,
        .pVertexAttributeDescriptions    = attributes,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,

        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkViewport viewport_ = {
        .x = 0.f,
        .y = 0.f,

        .width  = (Flt32)window_->_render_context._swapchain_extent.width,
        .height = (Flt32)window_->_render_context._swapchain_extent.height,

        .minDepth = 0.f,
        .maxDepth = 1.f,
    };

    VkRect2D scissor_ = {
        .offset = {0.f, 0.f},
        .extent = window_->_render_context._swapchain_extent,
    };

    VkDynamicState dynamic_states_[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,

        .dynamicStateCount = VYTAL_ARRAY_SIZE(dynamic_states_),
        .pDynamicStates    = dynamic_states_,
    };

    VkPipelineViewportStateCreateInfo viewport_state_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,

        .viewportCount = 1,
        .pViewports    = &viewport_,

        .scissorCount = 1,
        .pScissors    = &scissor_,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,

        .rasterizerDiscardEnable = VK_FALSE,
        .lineWidth               = 1.f,
        .polygonMode             = raster_polygon_mode_,
        .cullMode                = raster_cull_mode_,
        .frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,

        .depthClampEnable        = VK_FALSE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp          = 0.f,
        .depthBiasSlopeFactor    = 0.f,
    };

    VkPipelineMultisampleStateCreateInfo multisample_state_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,

        .sampleShadingEnable  = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading     = 1.f,
        .pSampleMask          = NULL,

        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment_state_ = {
        .blendEnable    = color_blend_enabled_,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,

        .srcColorBlendFactor = src_color_blend_factor_,
        .dstColorBlendFactor = dst_color_blend_factor_,
        .colorBlendOp        = VK_BLEND_OP_ADD,

        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
    };

    VkPipelineColorBlendStateCreateInfo color_blend_state_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,

        .logicOpEnable = VK_FALSE,
        .logicOp       = VK_LOGIC_OP_COPY,

        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment_state_,

        .blendConstants =
            {(type == RENDERER_GRAPHICS_PIPELINE_TYPE_TRANSPARENT) ? 1.0f : 0.0f,
             (type == RENDERER_GRAPHICS_PIPELINE_TYPE_TRANSPARENT) ? 1.0f : 0.0f,
             (type == RENDERER_GRAPHICS_PIPELINE_TYPE_TRANSPARENT) ? 1.0f : 0.0f,
             (type == RENDERER_GRAPHICS_PIPELINE_TYPE_TRANSPARENT) ? 1.0f : 0.0f},

    };

    VkStencilOpState stencil_op_state_ = {
        .failOp      = VK_STENCIL_OP_KEEP,
        .passOp      = VK_STENCIL_OP_REPLACE,
        .depthFailOp = VK_STENCIL_OP_KEEP,
        .compareOp   = VK_COMPARE_OP_ALWAYS,

        .compareMask = 0xff,
        .writeMask   = 0xff,

        .reference = 1,
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,

        .depthTestEnable  = VK_TRUE,
        .depthWriteEnable = depth_write_enabled_,

        .depthCompareOp = VK_COMPARE_OP_LESS,

        .depthBoundsTestEnable = VK_FALSE,
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f,

        .stencilTestEnable = VK_TRUE,
        .front             = stencil_op_state_,
        .back              = stencil_op_state_,
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info_ = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,

        .setLayoutCount = 1,
        .pSetLayouts    = &window_->_render_context._graphics_desc_set_layout._handle,

        .pushConstantRangeCount = 0,
        .pPushConstantRanges    = NULL,
    };

    if (vkCreatePipelineLayout(context_->_device, &pipeline_layout_info_, NULL, out_pipeline_layout) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_PIPELINE_CONSTRUCT_FAILED;

    VkGraphicsPipelineCreateInfo pipeline_info_ = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,

        .stageCount = VYTAL_ARRAY_SIZE(shader_stage_infos_),
        .pStages    = shader_stage_infos_,

        .pVertexInputState   = &vertex_input_state_info_,
        .pInputAssemblyState = &input_assembly_state_info_,
        .pViewportState      = &viewport_state_info_,
        .pRasterizationState = &rasterization_state_info_,
        .pMultisampleState   = &multisample_state_info_,
        .pDepthStencilState  = &depth_stencil_state_info_,
        .pColorBlendState    = &color_blend_state_info_,
        .pDynamicState       = &dynamic_state_info_,

        .layout = *out_pipeline_layout,

        .renderPass = window_->_render_context._render_pass,
        .subpass    = 0,
    };

    if (vkCreateGraphicsPipelines(context_->_device, VK_NULL_HANDLE, 1, &pipeline_info_, NULL, out_pipeline) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_PIPELINE_CONSTRUCT_FAILED;

    RendererBackendResult destruct_vert_shader_module_ = renderer_backend_vulkan_helpers_destruct_shader_module(context_, vert_shader_);
    if (destruct_vert_shader_module_ != RENDERER_BACKEND_SUCCESS)
        return destruct_vert_shader_module_;

    RendererBackendResult destruct_frag_shader_module_ = renderer_backend_vulkan_helpers_destruct_shader_module(context_, frag_shader_);
    if (destruct_frag_shader_module_ != RENDERER_BACKEND_SUCCESS)
        return destruct_frag_shader_module_;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_destruct_graphics_pipeline(
    const VoidPtr    context,
    VkPipeline       pipeline,
    VkPipelineLayout pipeline_layout) {
    if (!context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    if (pipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(context_->_device, pipeline, NULL);

    if (pipeline_layout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(context_->_device, pipeline_layout, NULL);

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_transition_image_layout(
    const VoidPtr            context,
    const VoidPtr            window,
    const VkImage            image,
    const VkFormat           format,
    const VkImageLayout      old_layout,
    const VkImageLayout      new_layout,
    const VkImageAspectFlags aspect_flags,
    const UInt32             mip_levels) {
    if (!context || !image || !mip_levels) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)window;

    VkCommandBuffer      *cmd_buffers_;
    RendererBackendResult begin_cmds_ = renderer_backend_vulkan_helpers_begin_single_time_commands(
        context_,
        &window_->_render_context._graphics_cmd_pool,
        1,
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        &cmd_buffers_);
    if (begin_cmds_ != RENDERER_BACKEND_SUCCESS)
        return begin_cmds_;

    VkImageMemoryBarrier barrier_ = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,

        .oldLayout = old_layout,
        .newLayout = new_layout,

        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image = image,

        .subresourceRange = {
            .aspectMask = aspect_flags,

            .baseMipLevel = 0,
            .levelCount   = mip_levels,

            .baseArrayLayer = 0,
            .layerCount     = 1,
        },

        .srcAccessMask = 0,
        .dstAccessMask = 0,
    };

    VkPipelineStageFlags src_stage_;
    VkPipelineStageFlags dst_stage_;

    if ((old_layout == VK_IMAGE_LAYOUT_UNDEFINED) && (new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)) {
        barrier_.srcAccessMask = 0;
        barrier_.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        src_stage_ = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage_ = VK_PIPELINE_STAGE_TRANSFER_BIT;

    } else if ((old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)) {
        barrier_.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier_.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        src_stage_ = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage_ = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    } else if ((old_layout == VK_IMAGE_LAYOUT_UNDEFINED) && (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)) {
        barrier_.srcAccessMask = 0;
        barrier_.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        src_stage_ = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage_ = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }

    else
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_TRANSITION_IMAGE_LAYOUT_FAILED;

    vkCmdPipelineBarrier(
        cmd_buffers_[0],
        src_stage_, dst_stage_,
        0,
        /* memory           */ 0, NULL,
        /* buffer memory    */ 0, NULL,
        /* image memory     */ 1, &barrier_);

    RendererBackendResult end_cmds_ = renderer_backend_vulkan_helpers_end_single_time_commands(
        context_,
        &window_->_render_context._graphics_cmd_pool,
        context_->_queue_families._graphics,
        1,
        cmd_buffers_);
    if (end_cmds_ != RENDERER_BACKEND_SUCCESS)
        return end_cmds_;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_search_supported_format(
    const VoidPtr              context,
    const VkFormat            *candidates,
    const size_t               candidate_count,
    const VkImageTiling        tiling,
    const VkFormatFeatureFlags features,
    VkFormat                  *out_format) {
    if (!context || !candidates || !candidate_count || !out_format) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    for (ByteSize i = 0; i < candidate_count; ++i) {
        VkFormat           format_ = candidates[i];
        VkFormatProperties properties_;
        vkGetPhysicalDeviceFormatProperties(context_->_gpu, format_, &properties_);

        if ((tiling == VK_IMAGE_TILING_LINEAR) && ((properties_.linearTilingFeatures & features) == features)) {
            *out_format = format_;
            return RENDERER_BACKEND_SUCCESS;
        }

        else if ((tiling == VK_IMAGE_TILING_OPTIMAL) && ((properties_.optimalTilingFeatures & features) == features)) {
            *out_format = format_;
            return RENDERER_BACKEND_SUCCESS;
        }
    }

    return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_SEARCH_SUPPORTED_FORMAT_FAILED;
}

RendererBackendResult renderer_backend_vulkan_helpers_search_depth_format(
    const VoidPtr context,
    VkFormat     *out_format) {
    if (!context || !out_format) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    VkFormat candidates_[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    return renderer_backend_vulkan_helpers_search_supported_format(
        context,
        candidates_,
        VYTAL_ARRAY_SIZE(candidates_),
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
        out_format);
}

RendererBackendResult renderer_backend_vulkan_helpers_search_memory_type(
    const VkPhysicalDevice      gpu,
    const UInt32                type_filter,
    const VkMemoryPropertyFlags properties,
    ByteSize                   *out_memory_type) {
    if (!out_memory_type) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    VkPhysicalDeviceMemoryProperties mem_properties_;
    vkGetPhysicalDeviceMemoryProperties(gpu, &mem_properties_);

    for (size_t i = 0; i < mem_properties_.memoryTypeCount; ++i) {
        if ((type_filter & (1 << i)) && ((mem_properties_.memoryTypes[i].propertyFlags & properties) == properties)) {
            *out_memory_type = i;
            return RENDERER_BACKEND_SUCCESS;
        }
    }

    return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_SEARCH_MEMORY_TYPE_FAILED;
}

RendererBackendResult renderer_backend_vulkan_helpers_begin_single_time_commands(
    const VoidPtr              context,
    const VkCommandPool       *pool,
    const UInt32               cmd_buffer_count,
    const VkCommandBufferLevel level,
    VkCommandBuffer          **out_buffers) {
    if (!context || !pool || !cmd_buffer_count || !out_buffers) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    VkCommandBuffer *cmd_buffers_ = NULL;
    if (memory_zone_allocate("renderer", sizeof(VkCommandBuffer) * cmd_buffer_count, (VoidPtr *)&cmd_buffers_, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_BEGIN_SINGLE_TIME_COMMANDS_FAILED;

    VkCommandBufferAllocateInfo alloc_info_ = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,

        .commandPool        = *pool,
        .commandBufferCount = cmd_buffer_count,

        .level = level,
    };

    if (vkAllocateCommandBuffers(context_->_device, &alloc_info_, cmd_buffers_) != VK_SUCCESS) {
        if (memory_zone_deallocate("renderer", cmd_buffers_, sizeof(VkCommandBuffer) * cmd_buffer_count) != MEMORY_ZONE_SUCCESS)
            return RENDERER_BACKEND_ERROR_DEALLOCATION_FAILED;

        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_BEGIN_SINGLE_TIME_COMMANDS_FAILED;
    }

    VkCommandBufferBeginInfo begin_info_ = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    for (size_t i = 0; i < cmd_buffer_count; ++i)
        vkBeginCommandBuffer(cmd_buffers_[i], &begin_info_);

    *out_buffers = cmd_buffers_;
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_end_single_time_commands(
    const VoidPtr        context,
    const VkCommandPool *pool,
    const VkQueue        queue,
    const UInt32         cmd_buffer_count,
    VkCommandBuffer     *buffers) {
    if (!context || !pool || !cmd_buffer_count || !buffers) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    for (size_t i = 0; i < cmd_buffer_count; ++i)
        vkEndCommandBuffer(buffers[i]);

    // use fence to avoid stalling
    VkFenceCreateInfo fence_info_ = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };
    VkFence fence_;
    if (vkCreateFence(context_->_device, &fence_info_, NULL, &fence_) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_END_SINGLE_TIME_COMMANDS_FAILED;

    // and start submitting to the queue
    VkSubmitInfo submit_info_ = {
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = cmd_buffer_count,
        .pCommandBuffers    = buffers,
    };

    vkQueueSubmit(queue, 1, &submit_info_, fence_);
    vkQueueWaitIdle(queue);

    vkWaitForFences(context_->_device, 1, &fence_, VK_TRUE, UINT64_MAX);
    vkDestroyFence(context_->_device, fence_, NULL);

    // when done, deallocate the command buffers
    vkFreeCommandBuffers(context_->_device, *pool, cmd_buffer_count, buffers);
    if (memory_zone_deallocate("renderer", buffers, sizeof(VkCommandBuffer) * cmd_buffer_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_DEALLOCATION_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_read_shader_file(
    ConstStr  filepath,
    ByteSize *out_shader_size,
    UInt32  **out_shader_code) {
    if (!filepath || !out_shader_size || !out_shader_code) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    File file_ = {0};
    if (platform_filesystem_open_file(&file_, filepath, FILE_IO_MODE_READ, FILE_MODE_BINARY) != FILE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_SHADER_FILE_OPEN_FAILED;

    if (platform_filesystem_read_binary_uint32(&file_, out_shader_size, out_shader_code) != FILE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_SHADER_FILE_READ_FAILED;
    *out_shader_size *= sizeof(UInt32);

    if (platform_filesystem_close_file(&file_) != FILE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_SHADER_FILE_CLOSE_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}
