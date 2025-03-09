#include "vulkan_helpers.h"

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
        return RENDERER_BACKEND_ERROR_VULKAN_HELPERS_IMAGE_VIEWS_CONSTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_helpers_destruct_image_view(const VoidPtr context, VkImageView image_view) {
    if (!context || (image_view == VK_NULL_HANDLE)) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    vkDestroyImageView(context_->_device, image_view, NULL);
    return RENDERER_BACKEND_SUCCESS;
}
