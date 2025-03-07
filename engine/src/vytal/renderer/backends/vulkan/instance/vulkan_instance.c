#include "vulkan_instance.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

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

RendererBackendResult _renderer_backend_vulkan_instance_get_extensions(UInt32 *extension_count, ConstStr **extensions, const Bool validation_layer_enabled) {
    if (!extension_count || !extensions) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    UInt32    extension_count_ = 0;
    ConstStr *extentions_      = glfwGetRequiredInstanceExtensions(&extension_count_);
    if (!extentions_)
        return RENDERER_BACKEND_ERROR_VULKAN_EXTS_FETCH_FAILED;

    memcpy(*extensions, extentions_, sizeof(ConstStr) * extension_count_);

    if (validation_layer_enabled)
        (*extensions)[extension_count_++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    (*extension_count) = extension_count_;

    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult _renderer_backend_vulkan_instance_get_layers(UInt32 *layer_count, VkLayerProperties **layers, const Bool validation_layer_enabled) {
    if (!layer_count || !layers) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    UInt32   required_layer_count_ = 0;
    ConstStr required_layers_[]    = {0};

    if (validation_layer_enabled) {
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

    VkLayerProperties *layers_ = calloc(1, sizeof(VkLayerProperties) * layer_count_);
    get_layers_                = vkEnumerateInstanceLayerProperties(&layer_count_, layers_);
    if (get_layers_ != VK_SUCCESS) {
        free(layers_);
        layers_ = NULL;
    }

    ByteSize supported_layer_count_ = 0;
    for (ByteSize i = 0; i < required_layer_count_; ++i) {
        for (ByteSize j = 0; j < layer_count_; ++j) {
            if (!strcmp(layers_[j].layerName, required_layers_[i])) {
                memcpy(&(*layers)[supported_layer_count_++], &layers_[j], sizeof(VkLayerProperties));
                break;
            }
        }
    }

    free(layers_);
    layers_ = NULL;

    (*layer_count) = supported_layer_count_;
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_instance_construct(const Bool validation_layer_enabled, VoidPtr out_debug_msg_info, VoidPtr *out_instance) {
    if (!out_instance) return RENDERER_BACKEND_ERROR_INVALID_PARAM;

    UInt32             layer_count_     = 0;
    UInt32             extension_count_ = 0;
    VkLayerProperties *layers_          = calloc(1, sizeof(VkLayerProperties) * 16);
    ConstStr          *extensions_      = calloc(1, sizeof(ConstStr) * 64);

    RendererBackendResult get_exts_ = _renderer_backend_vulkan_instance_get_extensions(&extension_count_, &extensions_, validation_layer_enabled);
    if (get_exts_ != RENDERER_BACKEND_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_INSTANCE_CONSTRUCT_FAILED;

    RendererBackendResult get_layers_ = _renderer_backend_vulkan_instance_get_layers(&layer_count_, &layers_, validation_layer_enabled);
    if (get_layers_ != RENDERER_BACKEND_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_INSTANCE_CONSTRUCT_FAILED;

    ConstStr supported_layer_names_[16] = {0};
    for (ByteSize i = 0; i < layer_count_; ++i)
        supported_layer_names_[i] = layers_[i].layerName;

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

    memcpy(out_debug_msg_info, &debug_messenger_info_, sizeof(VkDebugUtilsMessengerCreateInfoEXT));

    VkInstanceCreateInfo create_info_ = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,

        .pApplicationInfo = &app_info_,

        .enabledExtensionCount   = extension_count_,
        .ppEnabledExtensionNames = extensions_,

        .enabledLayerCount   = layer_count_,
        .ppEnabledLayerNames = supported_layer_names_,

        .pNext = validation_layer_enabled ? (VoidPtr)&debug_messenger_info_ : NULL,
    };

    VkResult create_instance_ = vkCreateInstance(&create_info_, NULL, (VkInstance *)out_instance);
    if (create_instance_ != VK_SUCCESS)
        return RENDERER_BACKEND_ERROR_VULKAN_INSTANCE_CONSTRUCT_FAILED;

    free(layers_);
    free(extensions_);
    return RENDERER_BACKEND_SUCCESS;
}

RendererBackendResult renderer_backend_vulkan_instance_destruct(VoidPtr instance) {
    if (!instance) return RENDERER_BACKEND_ERROR_INVALID_PARAM;
    VkInstance instance_ = (VkInstance)instance;

    vkDestroyInstance(instance_, NULL);
    instance_ = VK_NULL_HANDLE;

    return RENDERER_BACKEND_SUCCESS;
}