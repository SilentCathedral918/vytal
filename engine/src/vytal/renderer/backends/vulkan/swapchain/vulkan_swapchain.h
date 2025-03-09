#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/backends/backend_vulkan.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_swapchain_construct(const VoidPtr context, VoidPtr *out_window);
VYTAL_API RendererBackendResult renderer_backend_vulkan_swapchain_reconstruct(const VoidPtr context, VoidPtr *out_window);
VYTAL_API RendererBackendResult renderer_backend_vulkan_swapchain_cleanup(const VoidPtr context, VoidPtr *out_window);
VYTAL_API RendererBackendResult renderer_backend_vulkan_swapchain_destruct(const VoidPtr context, VoidPtr *out_window);
