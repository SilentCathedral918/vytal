#include "vulkan_device.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Window_Handle {
    GLFWwindow *_handle;

    VkSurfaceKHR   _surface;
    VkSwapchainKHR _swapchain;
    VkFramebuffer *_frame_buffers;
    VkImageView   *_image_views;
    VkExtent2D     _extent;

    ByteSize _memory_size;
};

RendererBackendResult renderer_backend_vulkan_device_select_gpu(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    UInt32            gpu_candidate_count_ = 0;
    VkPhysicalDevice *gpu_candidates_      = VK_NULL_HANDLE;
    VkInstance        instance_            = (VkInstance)context_->_instance;

    VkResult get_all_gpus_ = vkEnumeratePhysicalDevices(instance_, &gpu_candidate_count_, NULL);
    if (get_all_gpus_ != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GPU_SELECT_FAILED;

    gpu_candidates_ = calloc(gpu_candidate_count_, sizeof(VkPhysicalDevice));
    if (!gpu_candidates_)
        return RENDERER_BACKEND_ERROR_VULKAN_GPU_SELECT_FAILED;

    get_all_gpus_ = vkEnumeratePhysicalDevices(instance_, &gpu_candidate_count_, gpu_candidates_);
    if (get_all_gpus_ != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_GPU_SELECT_FAILED;

    UInt32           highest_score_  = 0;
    VkPhysicalDevice best_candidate_ = VK_NULL_HANDLE;

    for (ByteSize i = 0; i < gpu_candidate_count_; ++i) {
        VkPhysicalDevice                 candidate_ = gpu_candidates_[i];
        VkPhysicalDeviceProperties       props_;
        VkPhysicalDeviceMemoryProperties mem_props_;

        vkGetPhysicalDeviceProperties(candidate_, &props_);
        vkGetPhysicalDeviceMemoryProperties(candidate_, &mem_props_);

        UInt32 score_ = 0;

        // discrete GPU is preferred
        score_ += (props_.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ? 1000 : 0;

        // higher VRAM = better (score per MB of VRAM)
        for (ByteSize j = 0; j < (ByteSize)mem_props_.memoryHeapCount; ++j) {
            if (mem_props_.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                score_ += mem_props_.memoryHeaps[j].size / (1024 * 1024);
            }
        }

        // higher max image dimension = better (better texture display)
        score_ += props_.limits.maxImageDimension2D;

        // higher max sampler alloc count = better (better for shaders)
        score_ += props_.limits.maxSamplerAllocationCount / 10;

        // make sure that queue family is supported
        {
            UInt32 queue_family_count_ = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(candidate_, &queue_family_count_, NULL);
            VkQueueFamilyProperties *queue_families_ = calloc(queue_family_count_, sizeof(VkQueueFamilyProperties));
            vkGetPhysicalDeviceQueueFamilyProperties(candidate_, &queue_family_count_, queue_families_);

            Bool graphics_queue_supported_ = false;
            Bool compute_queue_supported_  = false;
            Bool present_queue_supported_  = false;

            for (ByteSize j = 0; j < queue_family_count_; ++j) {
                if (queue_families_[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
                    compute_queue_supported_ = true;

                if (queue_families_[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    graphics_queue_supported_ = true;

                VkBool32 present_supported_ = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(candidate_, j, context_->_first_window->_surface, &present_supported_);
                if (present_supported_ == VK_TRUE)
                    present_queue_supported_ = true;
            }
            free(queue_families_);

            if (!compute_queue_supported_ || !graphics_queue_supported_ || !present_queue_supported_)
                continue;  // skip since this GPU lacks support for essential queues (compute, graphics, present)
        }

        // make sure that required extensions are supported
        {
            UInt32   extension_count_;
            ConstStr required_extensions_[]    = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
            Bool     all_extensions_supported_ = true;

            vkEnumerateDeviceExtensionProperties(candidate_, NULL, &extension_count_, NULL);
            VkExtensionProperties *extensions_ = calloc(extension_count_, sizeof(VkExtensionProperties));
            vkEnumerateDeviceExtensionProperties(candidate_, NULL, &extension_count_, extensions_);

            for (ByteSize j = 0; j < VYTAL_ARRAY_SIZE(required_extensions_); ++j) {
                Bool ext_found_ = false;

                for (ByteSize k = 0; k < extension_count_; ++k) {
                    if (!strcmp(required_extensions_[j], extensions_[k].extensionName)) {
                        ext_found_ = true;
                        break;
                    }
                }

                if (!ext_found_) {
                    all_extensions_supported_ = false;
                    break;
                }
            }
            free(extensions_);

            if (!all_extensions_supported_)
                continue;  // skip since this GPU lacks support for required extensions
        }

        // candidate breaking the higest score becomes best candidate
        if (score_ > highest_score_) {
            best_candidate_ = candidate_;
            highest_score_  = score_;

            context_->_workgroup_sizes._x = props_.limits.maxComputeWorkGroupSize[0];
            context_->_workgroup_sizes._y = props_.limits.maxComputeWorkGroupSize[1];
            context_->_workgroup_sizes._z = props_.limits.maxComputeWorkGroupSize[2];
        }
    }
    free(gpu_candidates_);

    if (best_candidate_ == VK_NULL_HANDLE)
        return RENDERER_BACKEND_ERROR_VULKAN_GPU_SELECT_FAILED;

    context_->_gpu = best_candidate_;
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_queue_search_families(const VoidPtr context, VoidPtr out_queue_families) {
    if (!context || !out_queue_families) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)context;

    UInt32 queue_family_count_ = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context_->_gpu, &queue_family_count_, NULL);
    VkQueueFamilyProperties *queue_families_ = calloc(queue_family_count_, sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(context_->_gpu, &queue_family_count_, queue_families_);

    QueueFamilies families_ = {
        ._graphics_index = -1,
        ._compute_index  = -1,
        ._present_index  = -1,
    };

    for (ByteSize i = 0; i < queue_family_count_; ++i) {
        VkBool32 present_supported_ = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(context_->_gpu, i, context_->_first_window->_surface, &present_supported_);

        if ((queue_families_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (families_._graphics_index == -1))
            families_._graphics_index = i;

        else if ((queue_families_[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && (families_._compute_index == -1) && (i != families_._graphics_index))
            families_._compute_index = i;

        else if ((present_supported_ == VK_TRUE) && (families_._present_index == -1) && (i != families_._graphics_index))
            families_._present_index = i;
    }

    // fallback: in case of no separate compute/present queue -> re-use graphics queue
    {
        if (families_._compute_index == -1)
            families_._compute_index = families_._graphics_index;

        if (families_._present_index == -1)
            families_._present_index = families_._graphics_index;
    }

    memcpy(out_queue_families, &families_, sizeof(QueueFamilies));
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_device_construct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    QueueFamilies families_;
    if (renderer_backend_vulkan_queue_search_families(context_, &families_) != RENDERER_BACKEND_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_QUEUE_FAMILIES_SEARCH_FAILED;

    UInt32 family_indices_[] = {
        families_._graphics_index,
        families_._compute_index,
        families_._present_index,
    };

    // remove duplicates the family indices (Vulkan does not want that)
    UInt32 unique_family_count_ = 0;
    for (ByteSize i = 0; i < VYTAL_ARRAY_SIZE(family_indices_); ++i) {
        Bool index_exists_ = false;
        for (ByteSize j = 0; j < unique_family_count_; ++j) {
            if (family_indices_[j] == family_indices_[i]) {
                index_exists_ = true;
                break;
            }
        }

        if (!index_exists_)
            family_indices_[unique_family_count_++] = family_indices_[i];
    }

    // costruct a queue for every unique queue family indices
    Flt32                   queue_priority_                                        = 1.0f;
    VkDeviceQueueCreateInfo queue_create_infos_[VYTAL_ARRAY_SIZE(family_indices_)] = {0};
    for (ByteSize i = 0; i < unique_family_count_; ++i) {
        queue_create_infos_[i] = (VkDeviceQueueCreateInfo){
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount       = 1,
            .queueFamilyIndex = family_indices_[i],
            .pQueuePriorities = &queue_priority_,
        };
    }

    // enable sampler_anisotropy feature
    VkPhysicalDeviceFeatures device_features_ = {
        .samplerAnisotropy = VK_TRUE,
        .fillModeNonSolid  = VK_TRUE,
    };

    // swapchain extension is required
    ConstStr required_extensions_[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    ConstStr *layer_names_ = calloc(context_->_layer_count, sizeof(ConstStr));
    for (ByteSize i = 0; i < context_->_layer_count; ++i)
        layer_names_[i] = context_->_layers[i].layerName;

    // construct the device
    VkDeviceCreateInfo device_info_ = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,

        .pEnabledFeatures = &device_features_,

        .enabledExtensionCount   = VYTAL_ARRAY_SIZE(required_extensions_),
        .ppEnabledExtensionNames = required_extensions_,

        .queueCreateInfoCount = unique_family_count_,
        .pQueueCreateInfos    = queue_create_infos_,

        .enabledLayerCount   = context_->_layer_count,
        .ppEnabledLayerNames = layer_names_,
    };

    VkResult res_ = vkCreateDevice(context_->_gpu, &device_info_, NULL, &context_->_device);
    if (res_ != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_DEVICE_CONSTRUCT_FAILED;

    // assign the queues...
    vkGetDeviceQueue(context_->_device, families_._compute_index, 0, &context_->_queue_families._compute);
    vkGetDeviceQueue(context_->_device, families_._graphics_index, 0, &context_->_queue_families._graphics);
    vkGetDeviceQueue(context_->_device, families_._present_index, 0, &context_->_queue_families._present);

    // ...and their indices
    context_->_queue_families._compute_index  = families_._compute_index;
    context_->_queue_families._graphics_index = families_._graphics_index;
    context_->_queue_families._present_index  = families_._present_index;

    free(layer_names_);
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_device_destruct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    vkDestroyDevice(context_->_device, NULL);
    context_->_device = VK_NULL_HANDLE;

    return RENDERER_BACKEND_SUCCESS;
}
