#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/backends/backend_vulkan.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_startup(Window *out_first_window, ConstStr shaders_filepath, RendererBackend *out_backend);
VYTAL_API RendererBackendResult renderer_backend_vulkan_shutdown(RendererBackend backend);
VYTAL_API RendererBackendResult renderer_backend_vulkan_begin_frame(void);
VYTAL_API RendererBackendResult renderer_backend_vulkan_end_frame(void);
VYTAL_API RendererBackendResult renderer_backend_vulkan_render(void);

VYTAL_API RendererBackendResult renderer_backend_vulkan_add_window(RendererBackend backend, Window *out_window);
VYTAL_API RendererBackendResult renderer_backend_vulkan_remove_window(RendererBackend backend, Window *out_window);
