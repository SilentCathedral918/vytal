#include "backend_vulkan.h"

#include <string.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/renderer/backends/vulkan/instance/vulkan_instance.h"
#include "vytal/renderer/backends/vulkan/window/vulkan_window.h"

typedef struct QueueFamilies {
    VkQueue _compute;
    UInt32  _compute_index;

    VkQueue _graphics;
    UInt32  _graphics_index;

    VkQueue _present;
    UInt32  _present_index;
} QueueFamilies;

typedef struct DescriptorSetLayout {
    UInt32 _id;
    UInt32 _ref_count;

    VkDescriptorSetLayout _handle;
} DescriptorSetLayout;

typedef struct DescriptorSet {
    UInt32 _id;
    UInt32 _ref_count;

    VkDescriptorSet _handle;
} DescriptorSet;

typedef struct Renderer_Texture {
    VkImage     _image;
    VkImageView _image_view;

    VkDeviceMemory _memory;
    VkDeviceSize   _size;

    UInt32    _mip_levels;
    VkSampler _sampler;
} RendererTexture;

typedef struct Renderer_Backend_Vulkan_Context {
    Bool _validation_layer_enabled;

    VkDebugUtilsMessengerEXT            _debug_messenger;
    VkDebugUtilsMessengerCreateInfoEXT  _debug_messenger_info;
    PFN_vkCreateDebugUtilsMessengerEXT  _pfn_create_debug_messenger;
    PFN_vkDestroyDebugUtilsMessengerEXT _pfn_destroy_debug_messenger;

    VkInstance _instance;

    VkSurfaceKHR             _surface;
    VkSurfaceCapabilitiesKHR _surface_capabilities;

    VkPhysicalDevice _gpu;
    VkDevice         _device;

    struct WorkGroupSizes {
        size_t _x;
        size_t _y;
        size_t _z;
    } _workgroup_sizes;

    QueueFamilies _queue_families;

    VkSwapchainKHR     _curr_swapchain;
    VkSwapchainKHR     _prev_swapchain;
    VkExtent2D         _swapchain_extent;
    VkSurfaceFormatKHR _swapchain_surface_format;
    VkPresentModeKHR   _swapchain_present_mode;
    UInt32             _swapchain_image_count;
    VkImage           *_swapchain_images;
    VkImageView       *_swapchain_image_views;

    VkRenderPass _render_pass;

    VkImage        _depth_image;
    VkImageView    _depth_image_view;
    VkDeviceMemory _depth_image_memory;

    VkFramebuffer *_framebuffers;

    VkCommandPool        _compute_cmd_pool;
    VkCommandBuffer      _compute_cmd_buffer;
    VkDescriptorPool     _compute_desc_pool;
    DescriptorSetLayout *_compute_desc_set_layouts;
    UInt32               _compute_desc_set_layout_count;

    VkCommandPool       _graphics_cmd_pool;
    VkCommandBuffer    *_graphics_cmd_buffers;
    VkDescriptorPool    _graphics_desc_pool;
    DescriptorSetLayout _graphics_desc_set_layout;
    DescriptorSet      *_graphics_desc_sets;
    VkPipelineLayout   *_graphics_pipeline_layouts;
    VkPipeline         *_graphics_pipelines;
    RendererBuffer     *_graphics_ubos;
    VoidPtr            *_graphics_ubos_mapped;
    UInt32              _graphics_in_flight_fence_count;
    VkFence            *_graphics_in_flight_fences;
    VkSemaphore        *_graphics_image_available_semaphores;
    VkSemaphore        *_graphics_render_complete_semaphores;

    GraphicsPipelineType _active_pipeline;
    RendererTexture      _default_texture;
    UInt32               _frame_index;

} RendererBackendVulkanContext;

RendererBackendResult _renderer_backend_vulkan_load_debug_functions(RendererBackend *out_backend) {
    if (!out_backend) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    RendererBackendVulkanContext *context_ = (*out_backend)->_context;

    context_->_pfn_create_debug_messenger  = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context_->_instance, "vkCreateDebugUtilsMessengerEXT");
    context_->_pfn_destroy_debug_messenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context_->_instance, "vkDestroyDebugUtilsMessengerEXT");

    if (!context_->_pfn_create_debug_messenger || !context_->_pfn_destroy_debug_messenger)
        return RENDERER_BACKEND_ERROR_VULKAN_DEBUG_FUNCS_LOAD_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_startup(RendererBackend *out_backend) {
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

    RendererBackendResult construct_instance_ = renderer_backend_vulkan_instance_construct(context_->_validation_layer_enabled, (VoidPtr *)&context_->_debug_messenger_info, (VoidPtr *)&context_->_instance);
    if (construct_instance_ != RENDERER_BACKEND_SUCCESS)
        return construct_instance_;

    if (context_->_validation_layer_enabled) {
        RendererBackendResult load_debug_funcs_ = _renderer_backend_vulkan_load_debug_functions(out_backend);
        if (load_debug_funcs_ != RENDERER_BACKEND_SUCCESS)
            return load_debug_funcs_;

        VkResult create_debug_msg_ = context_->_pfn_create_debug_messenger(context_->_instance, &context_->_debug_messenger_info, NULL, &context_->_debug_messenger);
        if (create_debug_msg_ != VK_SUCCESS)
            return RENDERER_BACKEND_ERROR_VULKAN_DEBUG_MSG_CONSTRUCT_FAILED;
    }

    (*out_backend)->_memory_size = alloc_size_;
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_shutdown(RendererBackend backend) {
    if (!backend) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;

    RendererBackendVulkanContext *context_ = backend->_context;

    if (context_->_validation_layer_enabled && context_->_debug_messenger) {
        context_->_pfn_destroy_debug_messenger(context_->_instance, context_->_debug_messenger, NULL);
        context_->_debug_messenger = VK_NULL_HANDLE;
    }

    if (context_->_instance) {
        RendererBackendResult destruct_instance_ = renderer_backend_vulkan_instance_destruct((VoidPtr)context_->_instance);
        if (destruct_instance_ != RENDERER_BACKEND_SUCCESS)
            return destruct_instance_;
    }

    // deallocate renderer backend
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

    return renderer_backend_vulkan_window_construct(context_->_instance, (VoidPtr *)out_window);
}

RendererBackendResult renderer_backend_vulkan_remove_window(RendererBackend backend, Window *out_window) {
    if (!backend || !backend->_context) return RENDERER_BACKEND_ERROR_NOT_INITIALIZED;
    RendererBackendVulkanContext *context_ = backend->_context;

    return renderer_backend_vulkan_window_destruct(context_->_instance, (VoidPtr *)out_window);
}
