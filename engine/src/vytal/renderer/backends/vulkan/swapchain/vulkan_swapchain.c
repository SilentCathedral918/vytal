#include "vulkan_swapchain.h"

#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/misc/assertion/assertion.h"

struct Window_Handle {
    GLFWwindow *_handle;

    VkSurfaceKHR _surface;

    VkSwapchainKHR     _curr_swapchain;
    VkSwapchainKHR     _prev_swapchain;
    VkSurfaceFormatKHR _swapchain_surface_format;
    VkPresentModeKHR   _swapchain_present_mode;
    UInt32             _swapchain_image_count;
    VkImage           *_swapchain_images;
    VkImageView       *_swapchain_image_views;
    VkExtent2D         _swapchain_extent;

    VkFramebuffer *_frame_buffers;

    GraphicsPipelineType _active_pipeline;
    UInt32               _frame_index;

    ByteSize _memory_size;
};

VkCompositeAlphaFlagBitsKHR _renderer_backend_vulkan_swapchain_select_composite_alpha(VkSurfaceCapabilitiesKHR *capabilities) {
    VkCompositeAlphaFlagBitsKHR available_composite_alphas_[] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (ByteSize i = 0; i < VYTAL_ARRAY_SIZE(available_composite_alphas_); ++i) {
        if (capabilities->supportedCompositeAlpha & available_composite_alphas_[i])
            return available_composite_alphas_[i];
    }

    // fallback: resort to system default
    // but we need to make sure that system default is available
    {
        VYTAL_ASSERT_MESSAGE(capabilities->supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR, "no valid composite alpha mode available!");
        return VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    }
}

VkExtent2D _renderer_backend_vulkan_swapchain_select_swap_extent(GLFWwindow *window, VkSurfaceCapabilitiesKHR *capabilities) {
    if (capabilities->currentExtent.width != UINT32_MAX)
        return capabilities->currentExtent;

    else {
        Int32 width_, height_;
        glfwGetFramebufferSize(window, &width_, &height_);

        return (VkExtent2D){
            .width  = VYTAL_MATH_CLAMP((Int32)width_, capabilities->minImageExtent.width, capabilities->maxImageExtent.width),
            .height = VYTAL_MATH_CLAMP((Int32)height_, capabilities->minImageExtent.height, capabilities->maxImageExtent.height),
        };
    }
}

VkPresentModeKHR _renderer_backend_vulkan_swapchain_select_present_mode(VkPhysicalDevice gpu, VkSurfaceKHR surface) {
    UInt32 present_mode_count_ = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count_, NULL);

    VkPresentModeKHR *present_modes_ = calloc(present_mode_count_, sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count_, present_modes_);

    for (ByteSize i = 0; i < present_mode_count_; ++i) {
        VkPresentModeKHR present_mode_ = present_modes_[i];

        if (present_mode_ == VK_PRESENT_MODE_MAILBOX_KHR) {
            free(present_modes_);
            return present_mode_;
        }
    }

    // fallback: resort to widely-supported FIFO present mode
    free(present_modes_);
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR _renderer_backend_vulkan_swapchain_select_surface_format(VkPhysicalDevice gpu, VkSurfaceKHR surface) {
    VkFormat        ideal_format_      = VK_FORMAT_R8G8B8_SRGB;
    VkColorSpaceKHR ideal_color_space_ = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    UInt32 surface_format_count_ = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count_, NULL);

    VkSurfaceFormatKHR *surface_formats_ = calloc(surface_format_count_, sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count_, surface_formats_);

    for (ByteSize i = 0; i < surface_format_count_; ++i) {
        VkSurfaceFormatKHR format_ = surface_formats_[i];

        if ((format_.format == ideal_format_) && (format_.colorSpace == ideal_color_space_)) {
            free(surface_formats_);
            return format_;
        }
    }

    // fallback: resort to the first available surface formats
    VkSurfaceFormatKHR selected_format_ = surface_formats_[0];
    free(surface_formats_);
    return selected_format_;
}

RendererBackendResult renderer_backend_vulkan_swapchain_construct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    UInt32 family_indices_[] = {
        context_->_queue_families._graphics_index,
        context_->_queue_families._compute_index,
        context_->_queue_families._present_index,
    };

    VkCompositeAlphaFlagBitsKHR composite_alpha_ = _renderer_backend_vulkan_swapchain_select_composite_alpha(&context_->_surface_capabilities);
    VkExtent2D                  swap_extent_     = _renderer_backend_vulkan_swapchain_select_swap_extent(context_->_first_window->_handle, &context_->_surface_capabilities);
    VkPresentModeKHR            present_mode_    = _renderer_backend_vulkan_swapchain_select_present_mode(context_->_gpu, context_->_first_window->_surface);
    VkSurfaceFormatKHR          surface_format_  = _renderer_backend_vulkan_swapchain_select_surface_format(context_->_gpu, context_->_first_window->_surface);

    UInt32 image_count_ = context_->_surface_capabilities.minImageCount + 1;
    if ((context_->_surface_capabilities.maxImageCount > 0) && (image_count_ > context_->_surface_capabilities.maxImageCount))
        image_count_ = context_->_surface_capabilities.maxImageCount;

    Bool graphics_present_share_index_ = (context_->_queue_families._graphics_index == context_->_queue_families._present_index);

    VkSwapchainCreateInfoKHR swapchain_info_ = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,

        .surface = context_->_first_window->_surface,

        .minImageCount    = image_count_,
        .imageExtent      = swap_extent_,
        .imageArrayLayers = 1,  // alwayss 1 unless we are working with stereoscopic displays
        .imageFormat      = surface_format_.format,
        .imageColorSpace  = surface_format_.colorSpace,
        .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = graphics_present_share_index_ ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,

        .queueFamilyIndexCount = VYTAL_ARRAY_SIZE(family_indices_),
        .pQueueFamilyIndices   = family_indices_,

        .preTransform   = context_->_surface_capabilities.currentTransform,
        .compositeAlpha = composite_alpha_,
        .clipped        = VK_TRUE,
        .presentMode    = present_mode_,

        .oldSwapchain = window_->_prev_swapchain,
    };

    if (vkCreateSwapchainKHR(context_->_device, &swapchain_info_, NULL, &window_->_curr_swapchain) != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SWAPCHAIN_CONSTRUCT_FAILED;

    window_->_swapchain_present_mode   = present_mode_;
    window_->_swapchain_surface_format = surface_format_;
    window_->_swapchain_extent         = swap_extent_;

    // acquire swapchain images
    {
        UInt32 swapchain_image_count_ = 0;
        if (vkGetSwapchainImagesKHR(context_->_device, window_->_curr_swapchain, &swapchain_image_count_, NULL) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_SWAPCHAIN_CONSTRUCT_FAILED;

        window_->_swapchain_image_count = swapchain_image_count_;

        if (memory_zone_allocate("renderer", sizeof(VkImage) * swapchain_image_count_, (VoidPtr *)&window_->_swapchain_images, NULL) != MEMORY_ZONE_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_SWAPCHAIN_CONSTRUCT_FAILED;

        if (vkGetSwapchainImagesKHR(context_->_device, window_->_curr_swapchain, &swapchain_image_count_, window_->_swapchain_images) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_SWAPCHAIN_CONSTRUCT_FAILED;
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_swapchain_reconstruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    Int32 width_ = 0, height_ = 0;
    while (width_ == 0 || height_ == 0) {
        glfwGetFramebufferSize(window_->_handle, &width_, &height_);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(context_->_device);

    // store the current swapchain as old swapchain
    window_->_prev_swapchain = window_->_curr_swapchain;

    // cleanup and re-construct
    {
        RendererBackendResult cleanup_swapchain_ = renderer_backend_vulkan_swapchain_cleanup(context, out_window);
        if (cleanup_swapchain_ != RENDERER_BACKEND_SUCCESS)
            return cleanup_swapchain_;

        RendererBackendResult construct_swapchain_ = renderer_backend_vulkan_swapchain_construct(context, out_window);
        if (construct_swapchain_ != RENDERER_BACKEND_SUCCESS)
            return construct_swapchain_;
    }

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_swapchain_cleanup(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    if (window_->_curr_swapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(context_->_device, window_->_curr_swapchain, NULL);

    if (window_->_swapchain_image_views != NULL) {
        for (ByteSize i = 0; i < window_->_swapchain_image_count; ++i)
            vkDestroyImageView(context_->_device, window_->_swapchain_image_views[i], NULL);
    }

    if (memory_zone_deallocate("renderer", window_->_swapchain_images, sizeof(VkImage) * window_->_swapchain_image_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SWAPCHAIN_CONSTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_swapchain_destruct(const VoidPtr context, VoidPtr *out_window) {
    if (!context || !out_window) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;
    Window                        window_  = (Window)(*out_window);

    if (window_->_curr_swapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(context_->_device, window_->_curr_swapchain, NULL);

    if (window_->_prev_swapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(context_->_device, window_->_prev_swapchain, NULL);

    if (memory_zone_deallocate("renderer", window_->_swapchain_images, sizeof(VkImage) * window_->_swapchain_image_count) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_SWAPCHAIN_CONSTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}
