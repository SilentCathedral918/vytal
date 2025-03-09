#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/backends/backend_vulkan.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_window_construct(VoidPtr context, VoidPtr *out_window);
VYTAL_API RendererBackendResult renderer_backend_vulkan_window_destruct(VoidPtr context, VoidPtr *out_window);
