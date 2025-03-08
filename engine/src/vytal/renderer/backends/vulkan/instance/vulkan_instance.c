#include "vulkan_instance.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/memory/zone/memory_zone.h"

VKAPI_ATTR VkBool32 VKAPI_CALL _renderer_backend_vulkan_debug_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void                                       *pUserData) {
    fprintf(stderr, "%s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

RendererBackendResult _renderer_backend_vulkan_instance_get_extensions(RendererBackendVulkanContext *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    UInt32    extension_count_ = 0;
    ConstStr *extentions_      = glfwGetRequiredInstanceExtensions(&extension_count_);
    if (!extentions_)
        return RENDERER_BACKEND_ERROR_VULKAN_EXTS_FETCH_FAILED;

    memcpy(out_context->_extensions, extentions_, sizeof(ConstStr) * extension_count_);

    if (out_context->_validation_layer_enabled)
        out_context->_extensions[extension_count_++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    out_context->_extension_count = extension_count_;
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult _renderer_backend_vulkan_instance_get_layers(RendererBackendVulkanContext *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    UInt32   required_layer_count_ = 0;
    ConstStr required_layers_[]    = {0};

    if (out_context->_validation_layer_enabled) {
        required_layer_count_ = 1;
        required_layers_[0]   = "VK_LAYER_KHRONOS_validation";
    } else {
        required_layer_count_ = 0;
        required_layers_[0]   = 0;
    }

    UInt32   layer_count_ = 0;
    VkResult get_layers_  = vkEnumerateInstanceLayerProperties(&layer_count_, NULL);
    if (get_layers_ != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_LAYERS_FETCH_FAILED;

    VkLayerProperties *layers_ = calloc(layer_count_, sizeof(VkLayerProperties));
    get_layers_                = vkEnumerateInstanceLayerProperties(&layer_count_, layers_);
    if (get_layers_ != VK_SUCCESS) {
        free(layers_);
        layers_ = NULL;
    }

    ByteSize supported_layer_count_ = 0;
    for (ByteSize i = 0; i < required_layer_count_; ++i) {
        for (ByteSize j = 0; j < layer_count_; ++j) {
            if (!strcmp(layers_[j].layerName, required_layers_[i])) {
                memcpy(&(out_context->_layers[supported_layer_count_++]), &layers_[j], sizeof(VkLayerProperties));
                break;
            }
        }
    }

    free(layers_);
    layers_ = NULL;

    out_context->_layer_count = supported_layer_count_;
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_instance_construct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    RendererBackendResult get_exts_ = _renderer_backend_vulkan_instance_get_extensions(context_);
    if (get_exts_ != RENDERER_BACKEND_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_INSTANCE_CONSTRUCT_FAILED;

    RendererBackendResult get_layers_ = _renderer_backend_vulkan_instance_get_layers(context_);
    if (get_layers_ != RENDERER_BACKEND_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_INSTANCE_CONSTRUCT_FAILED;

    ConstStr supported_layer_names_[16] = {0};
    for (ByteSize i = 0; i < context_->_layer_count; ++i)
        supported_layer_names_[i] = context_->_layers[i].layerName;

    VkApplicationInfo app_info_ = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,

        .apiVersion = VK_API_VERSION_1_4,

        .pApplicationName   = "vulkan_renderer",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),

        .pEngineName   = "vytal_engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    };

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info_ = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,

        .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

        .pfnUserCallback = _renderer_backend_vulkan_debug_messenger_callback,
        .pUserData       = NULL,
    };
    context_->_debug_messenger_info = debug_messenger_info_;

    VkInstanceCreateInfo create_info_ = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,

        .pApplicationInfo = &app_info_,

        .enabledExtensionCount   = context_->_extension_count,
        .ppEnabledExtensionNames = context_->_extensions,

        .enabledLayerCount   = context_->_layer_count,
        .ppEnabledLayerNames = supported_layer_names_,

        .pNext = context_->_validation_layer_enabled ? (VoidPtr)&debug_messenger_info_ : NULL,
    };

    VkResult create_instance_ = vkCreateInstance(&create_info_, NULL, (VkInstance *)&context_->_instance);
    if (create_instance_ != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_INSTANCE_CONSTRUCT_FAILED;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_instance_destruct(VoidPtr *out_context) {
    if (!out_context) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    RendererBackendVulkanContext *context_ = (RendererBackendVulkanContext *)(*out_context);

    vkDestroyInstance(context_->_instance, NULL);
    context_->_instance = VK_NULL_HANDLE;

    return RENDERER_BACKEND_SUCCESS;
}