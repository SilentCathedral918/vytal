#include "vulkan_render_pass.h"

#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/renderer/backends/vulkan/helpers/vulkan_helpers.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_render_pass_construct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    VkAttachmentDescription color_attachment_ = {
        .format  = window_->_render_context._swapchain_surface_format.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,

        .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference color_attachment_ref_ = {
        .attachment = 0,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkFormat              depth_format_;
    RendererBackendResult search_depth_format_ = renderer_backend_vulkan_helpers_search_depth_format(context_, &depth_format_);
    if (search_depth_format_ != RENDERER_BACKEND_SUCCESS)
        return search_depth_format_;

    VkAttachmentDescription depth_attachment_ = {
        .format  = depth_format_,
        .samples = VK_SAMPLE_COUNT_1_BIT,

        .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference depth_attachment_ref_ = {
        .attachment = 1,
        .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentDescription attachments_[] = {
        color_attachment_,
        depth_attachment_,
    };

    VkSubpassDescription subpass_ = {
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &color_attachment_ref_,
        .pDepthStencilAttachment = &depth_attachment_ref_,
    };

    VkSubpassDependency dependency_ = {
        .srcSubpass    = VK_SUBPASS_EXTERNAL,
        .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,

        .dstSubpass    = 0,
        .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    VkRenderPassCreateInfo render_pass_info_ = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,

        .attachmentCount = VYTAL_ARRAY_SIZE(attachments_),
        .pAttachments    = attachments_,

        .subpassCount = 1,
        .pSubpasses   = &subpass_,

        .dependencyCount = 1,
        .pDependencies   = &dependency_,
    };

    if (vkCreateRenderPass(context_->_device, &render_pass_info_, NULL, &window_->_render_context._render_pass) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_RENDER_PASS_CONSTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_render_pass_destruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    if (window_->_render_context._render_pass != VK_NULL_HANDLE)
        vkDestroyRenderPass(context_->_device, window_->_render_context._render_pass, NULL);

    return RENDERER_BACKEND_SUCCESS;
}
