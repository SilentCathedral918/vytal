#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/backends/backend_vulkan.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_compute_command_buffers_construct(VoidPtr *out_context);
VYTAL_API RendererBackendResult renderer_backend_vulkan_compute_command_buffers_destruct(VoidPtr *out_context);

VYTAL_API RendererBackendResult renderer_backend_vulkan_graphics_command_buffers_construct(const VoidPtr context, VoidPtr *out_window);
VYTAL_API RendererBackendResult renderer_backend_vulkan_graphics_command_buffers_destruct(const VoidPtr context, VoidPtr *out_window);
