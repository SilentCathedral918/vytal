#pragma once

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/renderer.h"

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
    Window _first_window;

    UInt32            _layer_count;
    VkLayerProperties _layers[16];

    UInt32   _extension_count;
    ConstStr _extensions[64];

    Bool _validation_layer_enabled;

    VkDebugUtilsMessengerEXT            _debug_messenger;
    VkDebugUtilsMessengerCreateInfoEXT  _debug_messenger_info;
    PFN_vkCreateDebugUtilsMessengerEXT  _pfn_create_debug_messenger;
    PFN_vkDestroyDebugUtilsMessengerEXT _pfn_destroy_debug_messenger;

    VkInstance _instance;

    VkSurfaceCapabilitiesKHR _surface_capabilities;

    VkPhysicalDevice _gpu;
    VkDevice         _device;

    struct WorkGroupSizes {
        size_t _x;
        size_t _y;
        size_t _z;
    } _workgroup_sizes;

    VkCommandPool        _compute_cmd_pool;
    VkCommandBuffer      _compute_cmd_buffer;
    VkDescriptorPool     _compute_desc_pool;
    DescriptorSetLayout *_compute_desc_set_layouts;
    UInt32               _compute_desc_set_layout_count;

    QueueFamilies   _queue_families;
    RendererTexture _default_texture;
} RendererBackendVulkanContext;

typedef struct Renderer_Backend_Window_Context {
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

    VkRenderPass _render_pass;

    VkImage        _depth_image;
    VkImageView    _depth_image_view;
    VkDeviceMemory _depth_image_memory;

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
} RendererBackendWindowContext;
