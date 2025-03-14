#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/backends/backend_vulkan.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_descriptor_sets_construct(const VoidPtr context, VoidPtr *out_window);
VYTAL_API RendererBackendResult renderer_backend_vulkan_descriptor_sets_destruct(const VoidPtr context, VoidPtr *out_window);
