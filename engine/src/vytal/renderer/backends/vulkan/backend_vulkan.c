#include <string.h>

#include "backend_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/renderer/backends/vulkan/command_pools/vulkan_command_pools.h"
#include "vytal/renderer/backends/vulkan/depth_resources/vulkan_depth_resources.h"
#include "vytal/renderer/backends/vulkan/descriptor_pools/vulkan_descriptor_pools.h"
#include "vytal/renderer/backends/vulkan/descriptor_set_layouts/vulkan_descriptor_set_layouts.h"
#include "vytal/renderer/backends/vulkan/device/vulkan_device.h"
#include "vytal/renderer/backends/vulkan/framebuffers/vulkan_framebuffers.h"
#include "vytal/renderer/backends/vulkan/graphics_pipelines/vulkan_graphics_pipelines.h"
#include "vytal/renderer/backends/vulkan/instance/vulkan_instance.h"
#include "vytal/renderer/backends/vulkan/render_pass/vulkan_render_pass.h"
#include "vytal/renderer/backends/vulkan/swapchain/vulkan_swapchain.h"
#include "vytal/renderer/backends/vulkan/window/vulkan_window.h"

struct Window_Handle {
    GLFWwindow                  *_handle;
    RendererBackendWindowContext _render_context;

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

RendererBackendResult renderer_backend_vulkan_startup(Window *out_first_window, ConstStr shaders_filepath, RendererBackend *out_backend) {
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

    // configure shaders filepath
    if (*shaders_filepath)
        memcpy(context_->_shaders_filepath, shaders_filepath, LINE_BUFFER_MAX_SIZE);

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
        if (glfwCreateWindowSurface(context_->_instance, context_->_first_window->_handle, NULL, &context_->_first_window->_render_context._surface) != VK_SUCCESS)
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
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context_->_gpu, context_->_first_window->_render_context._surface, &capabilities_);
        context_->_surface_capabilities = capabilities_;
    }

    // command pools
    RendererBackendResult construct_cmd_pools_ = renderer_backend_vulkan_command_pools_construct((VoidPtr *)&context_);
    if (construct_cmd_pools_ != RENDERER_BACKEND_SUCCESS)
        return construct_cmd_pools_;

    // for the first window
    {
        // swapchain
        RendererBackendResult construct_swapchain_ = renderer_backend_vulkan_swapchain_construct(context_, (VoidPtr *)&context_->_first_window);
        if (construct_swapchain_ != RENDERER_BACKEND_SUCCESS)
            return construct_swapchain_;

        // swapchain image views
        RendererBackendResult construct_swapchain_image_views_ = renderer_backend_vulkan_swapchain_construct_image_views(context_, (VoidPtr *)&context_->_first_window);
        if (construct_swapchain_image_views_ != RENDERER_BACKEND_SUCCESS)
            return construct_swapchain_image_views_;

        // render pass
        RendererBackendResult construct_render_pass_ = renderer_backend_render_pass_construct(context_, (VoidPtr *)&context_->_first_window);
        if (construct_render_pass_ != RENDERER_BACKEND_SUCCESS)
            return construct_render_pass_;

        // depth resources
        RendererBackendResult construct_depth_resources_ = renderer_backend_vulkan_depth_resources_construct(context_, (VoidPtr *)&context_->_first_window);
        if (construct_depth_resources_ != RENDERER_BACKEND_SUCCESS)
            return construct_depth_resources_;

        // framebuffers
        RendererBackendResult construct_framebuffers_ = renderer_backend_vulkan_framebuffers_construct(context_, (VoidPtr *)&context_->_first_window);
        if (construct_framebuffers_ != RENDERER_BACKEND_SUCCESS)
            return construct_framebuffers_;
    }

    // descriptor pools
    RendererBackendResult construct_desc_pools_ = renderer_backend_vulkan_descriptor_pools_construct((VoidPtr *)&context_);
    if (construct_desc_pools_ != RENDERER_BACKEND_SUCCESS)
        return construct_desc_pools_;

    // descriptor set layouts
    RendererBackendResult construct_desc_set_layouts_ = renderer_backend_vulkan_descriptor_set_layouts_construct((VoidPtr *)&context_);
    if (construct_desc_set_layouts_ != RENDERER_BACKEND_SUCCESS)
        return construct_desc_set_layouts_;

    // for the first window
    {
        // graphics pipelines
        RendererBackendResult construct_graphics_pipelines_ = renderer_backend_vulkan_graphics_pipelines_construct(context_, (VoidPtr *)&context_->_first_window);
        if (construct_graphics_pipelines_ != RENDERER_BACKEND_SUCCESS)
            return construct_graphics_pipelines_;
    }

    (*out_backend)->_memory_size = alloc_size_;
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_shutdown(RendererBackend backend) {
    if (!backend) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;

    RendererBackendVulkanContext *context_ = backend->_context;

    // destruction for first window and its properties is already handled by the application that utilizes this engine
    // so no need to do it here

    // descriptor set layouts
    RendererBackendResult destruct_desc_set_layouts_ = renderer_backend_vulkan_descriptor_set_layouts_destruct((VoidPtr *)&context_);
    if (destruct_desc_set_layouts_ != RENDERER_BACKEND_SUCCESS)
        return destruct_desc_set_layouts_;

    // descriptor pools
    RendererBackendResult destruct_desc_pools_ = renderer_backend_vulkan_descriptor_pools_destruct((VoidPtr *)&context_);
    if (destruct_desc_pools_ != RENDERER_BACKEND_SUCCESS)
        return destruct_desc_pools_;

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
