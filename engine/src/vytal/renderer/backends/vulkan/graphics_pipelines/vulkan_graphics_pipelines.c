#include <stdio.h>

#include "vulkan_graphics_pipelines.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/renderer/backends/vulkan/helpers/vulkan_helpers.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_graphics_pipelines_construct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_                                 = (RendererBackendVulkanContext *)context;
    Window                        window_                                  = (Window)(*out_window);
    Char                          vert_filepath_[LINE_BUFFER_MAX_SIZE * 2] = {0};
    Char                          frag_filepath_[LINE_BUFFER_MAX_SIZE * 2] = {0};

    if (memory_zone_allocate("renderer", sizeof(VkPipelineLayout) * NUM_GRAPHICS_PIPELINES, (VoidPtr *)&window_->_render_context._graphics_pipeline_layouts, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_PIPELINE_CONSTRUCT_FAILED;

    if (memory_zone_allocate("renderer", sizeof(VkPipeline) * NUM_GRAPHICS_PIPELINES, (VoidPtr *)&window_->_render_context._graphics_pipelines, NULL) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_PIPELINE_CONSTRUCT_FAILED;

    VkVertexInputBindingDescription vertex_input_bindings_[] = {
        {
            .binding   = 0,
            .stride    = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
    };

    // textured --------------------------------------------------------- //

    VkVertexInputAttributeDescription vertex_input_attributes_textured_[] = {
        {
            .binding  = 0,
            .location = 0,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, _position),
        },
        {
            .binding  = 0,
            .location = 1,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, _normal),
        },
        {
            .binding  = 0,
            .location = 2,
            .format   = VK_FORMAT_R32G32_SFLOAT,
            .offset   = offsetof(Vertex, _texture_uv),
        },
    };

    snprintf(vert_filepath_, sizeof(vert_filepath_), "%s/%s", context_->_shaders_filepath, "graphics_pipelines/textured.vert.spv");
    snprintf(frag_filepath_, sizeof(frag_filepath_), "%s/%s", context_->_shaders_filepath, "graphics_pipelines/textured.frag.spv");

    RendererBackendResult construct_textured_pipeline_ = renderer_backend_vulkan_helpers_construct_graphics_pipeline(
        vert_filepath_,
        frag_filepath_,
        context_,
        window_,
        RENDERER_GRAPHICS_PIPELINE_TYPE_TEXTURED,
        VYTAL_ARRAY_SIZE(vertex_input_bindings_),
        vertex_input_bindings_,
        VYTAL_ARRAY_SIZE(vertex_input_attributes_textured_),
        vertex_input_attributes_textured_,
        &window_->_render_context._graphics_pipelines[RENDERER_GRAPHICS_PIPELINE_TYPE_TEXTURED],
        &window_->_render_context._graphics_pipeline_layouts[RENDERER_GRAPHICS_PIPELINE_TYPE_TEXTURED]);

    if (construct_textured_pipeline_ != RENDERER_BACKEND_SUCCESS)
        return construct_textured_pipeline_;

    // solid color ------------------------------------------------------- //

    VkVertexInputAttributeDescription vertex_input_attributes_solid_[] = {
        {
            .binding  = 0,
            .location = 0,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, _position),
        },
        {
            .binding  = 0,
            .location = 1,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, _normal),
        },
    };

    snprintf(vert_filepath_, sizeof(vert_filepath_), "%s/%s", context_->_shaders_filepath, "graphics_pipelines/solid.vert.spv");
    snprintf(frag_filepath_, sizeof(frag_filepath_), "%s/%s", context_->_shaders_filepath, "graphics_pipelines/solid.frag.spv");

    RendererBackendResult construct_solid_pipeline_ = renderer_backend_vulkan_helpers_construct_graphics_pipeline(
        vert_filepath_,
        frag_filepath_,
        context_,
        window_,
        RENDERER_GRAPHICS_PIPELINE_TYPE_SOLID_COLOR,
        VYTAL_ARRAY_SIZE(vertex_input_bindings_),
        vertex_input_bindings_,
        VYTAL_ARRAY_SIZE(vertex_input_attributes_solid_),
        vertex_input_attributes_solid_,
        &window_->_render_context._graphics_pipelines[RENDERER_GRAPHICS_PIPELINE_TYPE_SOLID_COLOR],
        &window_->_render_context._graphics_pipeline_layouts[RENDERER_GRAPHICS_PIPELINE_TYPE_SOLID_COLOR]);

    if (construct_solid_pipeline_ != RENDERER_BACKEND_SUCCESS)
        return construct_solid_pipeline_;

    // transparent ------------------------------------------------------- //

    VkVertexInputAttributeDescription vertex_input_attributes_transparent_[] = {
        {
            .binding  = 0,
            .location = 0,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, _position),
        },
        {
            .binding  = 0,
            .location = 1,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, _normal),
        },
        {
            .binding  = 0,
            .location = 2,
            .format   = VK_FORMAT_R32G32_SFLOAT,
            .offset   = offsetof(Vertex, _texture_uv),
        },
    };

    snprintf(vert_filepath_, sizeof(vert_filepath_), "%s/%s", context_->_shaders_filepath, "graphics_pipelines/transparent.vert.spv");
    snprintf(frag_filepath_, sizeof(frag_filepath_), "%s/%s", context_->_shaders_filepath, "graphics_pipelines/transparent.frag.spv");

    RendererBackendResult construct_transparent_pipeline_ = renderer_backend_vulkan_helpers_construct_graphics_pipeline(
        vert_filepath_,
        frag_filepath_,
        context_,
        window_,
        RENDERER_GRAPHICS_PIPELINE_TYPE_TRANSPARENT,
        VYTAL_ARRAY_SIZE(vertex_input_bindings_),
        vertex_input_bindings_,
        VYTAL_ARRAY_SIZE(vertex_input_attributes_transparent_),
        vertex_input_attributes_transparent_,
        &window_->_render_context._graphics_pipelines[RENDERER_GRAPHICS_PIPELINE_TYPE_TRANSPARENT],
        &window_->_render_context._graphics_pipeline_layouts[RENDERER_GRAPHICS_PIPELINE_TYPE_TRANSPARENT]);

    if (construct_transparent_pipeline_ != RENDERER_BACKEND_SUCCESS)
        return construct_transparent_pipeline_;

    // wireframe --------------------------------------------------------- //

    VkVertexInputAttributeDescription vertex_input_attributes_wireframe_[] = {
        {
            .binding  = 0,
            .location = 0,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, _position),
        },
    };

    snprintf(vert_filepath_, sizeof(vert_filepath_), "%s/%s", context_->_shaders_filepath, "graphics_pipelines/wireframe.vert.spv");
    snprintf(frag_filepath_, sizeof(frag_filepath_), "%s/%s", context_->_shaders_filepath, "graphics_pipelines/wireframe.frag.spv");

    RendererBackendResult construct_wireframe_pipeline_ = renderer_backend_vulkan_helpers_construct_graphics_pipeline(
        vert_filepath_,
        frag_filepath_,
        context_,
        window_,
        RENDERER_GRAPHICS_PIPELINE_TYPE_WIREFRAME,
        VYTAL_ARRAY_SIZE(vertex_input_bindings_),
        vertex_input_bindings_,
        VYTAL_ARRAY_SIZE(vertex_input_attributes_wireframe_),
        vertex_input_attributes_wireframe_,
        &window_->_render_context._graphics_pipelines[RENDERER_GRAPHICS_PIPELINE_TYPE_WIREFRAME],
        &window_->_render_context._graphics_pipeline_layouts[RENDERER_GRAPHICS_PIPELINE_TYPE_WIREFRAME]);

    if (construct_wireframe_pipeline_ != RENDERER_BACKEND_SUCCESS)
        return construct_wireframe_pipeline_;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_graphics_pipelines_destruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    for (ByteSize i = 0; i < NUM_GRAPHICS_PIPELINES; ++i) {
        RendererBackendResult destruct_pipeline_ = renderer_backend_vulkan_helpers_destruct_graphics_pipeline(
            context_,
            window_->_render_context._graphics_pipelines[i],
            window_->_render_context._graphics_pipeline_layouts[i]);

        if (destruct_pipeline_ != RENDERER_BACKEND_SUCCESS)
            return destruct_pipeline_;
    }

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_pipeline_layouts, sizeof(VkPipelineLayout) * NUM_GRAPHICS_PIPELINES) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_PIPELINE_DESTRUCT_FAILED;

    if (memory_zone_deallocate("renderer", window_->_render_context._graphics_pipelines, sizeof(VkPipeline) * NUM_GRAPHICS_PIPELINES) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GRAPHICS_PIPELINE_DESTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}
