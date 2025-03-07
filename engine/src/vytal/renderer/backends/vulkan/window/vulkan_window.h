#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/renderer.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_window_construct(VoidPtr instance, VoidPtr *out_window);
VYTAL_API RendererBackendResult renderer_backend_vulkan_window_destruct(VoidPtr instance, VoidPtr *out_window);
