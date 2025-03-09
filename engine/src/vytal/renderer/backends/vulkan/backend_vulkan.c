#include "backend_vulkan.h"

#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/renderer/backends/vulkan/command_pools/vulkan_command_pools.h"
#include "vytal/renderer/backends/vulkan/device/vulkan_device.h"
#include "vytal/renderer/backends/vulkan/instance/vulkan_instance.h"
#include "vytal/renderer/backends/vulkan/swapchain/vulkan_swapchain.h"
#include "vytal/renderer/backends/vulkan/window/vulkan_window.h"

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

RendererBackendResult _renderer_backend_vulkan_load_debug_functions(RendererBackend *out_backend) {
    if (!out_backend) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    RendererBackendVulkanContext *context_ = (*out_backend)->_context;

    context_->_pfn_create_debug_messenger  = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context_->_instance, "vkCreateDebugUtilsMessengerEXT");
    context_->_pfn_destroy_debug_messenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context_->_instance, "vkDestroyDebugUtilsMessengerEXT");

    if (!context_->_pfn_create_debug_messenger || !context_->_pfn_destroy_debug_messenger)
        return RENDERER_BACKEND_ERROR_VULKAN_DEBUG_FUNCS_LOAD_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_startup(Window *out_first_window, RendererBackend *out_backend) {
    if (out_backend && *out_backend) return RENDERER_BACKEND_ERROR_ALREADY_INITIALIZED;

    // allocate renderer backend
    ByteSize total_size_ = sizeof(struct Renderer_Backend) + sizeof(RendererBackendVulkanContext);
    ByteSize alloc_size_ = 0;
    if (memory_zone_allocate("renderer", total_size_, (VoidPtr *)out_backend, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_ALLOCATION_FAILED;
    memset(*out_backend, 0, alloc_size_);

    (*out_backend)->_context               = (VoidPtr)((BytePtr)(*out_backend) + sizeof(struct Renderer_Backend));
    RendererBackendVulkanContext *context_ = (*out_backend)->_context;

#if defined(VYTAL_VULKAN_VALIDATION_LAYERS_ENABLED)
    context_->_validation_layer_enabled = true;
#else
    context_->_validation_layer_enabled = false;
#endif

    // instance
    RendererBackendResult construct_instance_ = renderer_backend_vulkan_instance_construct((VoidPtr *)&context_);
    if (construct_instance_ != RENDERER_BACKEND_SUCCESS)
        return construct_instance_;

    // debug messenger
    if (context_->_validation_layer_enabled) {
        RendererBackendResult load_debug_funcs_ = _renderer_backend_vulkan_load_debug_functions(out_backend);
        if (load_debug_funcs_ != RENDERER_BACKEND_SUCCESS)
            return load_debug_funcs_;

        VkResult create_debug_msg_ = context_->_pfn_create_debug_messenger(context_->_instance, &context_->_debug_messenger_info, NULL, &context_->_debug_messenger);
        if (create_debug_msg_ != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DEBUG_MSG_CONSTRUCT_FAILED;
    }

    // first window, which is actually required for some Vulkan specifications
    // this window is the heart of the application that utilizes the engine
    {
        context_->_first_window = (*out_first_window);
        if (glfwCreateWindowSurface(context_->_instance, context_->_first_window->_handle, NULL, &context_->_first_window->_surface) != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_SURFACE_CONSTRUCT_FAILED;
    }

    // physical device (the GPU)
    RendererBackendResult select_gpu_ = renderer_backend_vulkan_device_select_gpu((VoidPtr *)&context_);
    if (select_gpu_ != RENDERER_BACKEND_SUCCESS)
        return select_gpu_;

    // logical device
    RendererBackendResult construct_device_ = renderer_backend_vulkan_device_construct((VoidPtr *)&context_);
    if (construct_device_ != RENDERER_BACKEND_SUCCESS)
        return construct_device_;

    // surface capabilities
    {
        VkSurfaceCapabilitiesKHR capabilities_;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context_->_gpu, context_->_first_window->_surface, &capabilities_);
        context_->_surface_capabilities = capabilities_;
    }

    // command pools
    RendererBackendResult construct_cmd_pools_ = renderer_backend_vulkan_command_pools_construct((VoidPtr *)&context_);
    if (construct_cmd_pools_ != RENDERER_BACKEND_SUCCESS)
        return construct_cmd_pools_;

    // swapchain, for the first window
    RendererBackendResult construct_swapchain_ = renderer_backend_vulkan_swapchain_construct(context_, (VoidPtr *)&context_->_first_window);
    if (construct_swapchain_ != RENDERER_BACKEND_SUCCESS)
        return construct_swapchain_;

    (*out_backend)->_memory_size = alloc_size_;
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_shutdown(RendererBackend backend) {
    if (!backend) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;

    RendererBackendVulkanContext *context_ = backend->_context;

    // destruction for first window and its properties is already handled by the application that utilizes this engine
    // so no need to do it here

    // command pools
    RendererBackendResult destruct_cmd_pools_ = renderer_backend_vulkan_command_pools_destruct((VoidPtr *)&context_);
    if (destruct_cmd_pools_ != RENDERER_BACKEND_SUCCESS)
        return destruct_cmd_pools_;

    // logical device
    RendererBackendResult destruct_device_ = renderer_backend_vulkan_device_destruct((VoidPtr *)&context_);
    if (destruct_device_ != RENDERER_BACKEND_SUCCESS)
        return destruct_device_;

    // debug messenger
    if (context_->_validation_layer_enabled && context_->_debug_messenger) {
        context_->_pfn_destroy_debug_messenger(context_->_instance, context_->_debug_messenger, NULL);
        context_->_debug_messenger = VK_NULL_HANDLE;
    }

    // instance
    if (context_->_instance) {
        RendererBackendResult destruct_instance_ = renderer_backend_vulkan_instance_destruct((VoidPtr *)&context_);
        if (destruct_instance_ != RENDERER_BACKEND_SUCCESS)
            return destruct_instance_;
    }

    // renderer backend
    if (memory_zone_deallocate("renderer", backend, backend->_memory_size) != MEMORY_ZONE_SUCCESS)
        return RENDERER_BACKEND_ERROR_DEALLOCATION_FAILED;
    backend = NULL;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_begin_frame(void) {
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_end_frame(void) {
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_render(void) {
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_add_window(RendererBackend backend, Window *out_window) {
    if (!backend || !backend->_context) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;
    RendererBackendVulkanContext *context_ = backend->_context;

    return renderer_backend_vulkan_window_construct(context_, (VoidPtr *)out_window);
}

RendererBackendResult renderer_backend_vulkan_remove_window(RendererBackend backend, Window *out_window) {
    if (!backend || !backend->_context) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;
    RendererBackendVulkanContext *context_ = backend->_context;

    return renderer_backend_vulkan_window_destruct(context_, (VoidPtr *)out_window);
}
