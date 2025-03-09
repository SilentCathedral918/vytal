#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/backends/backend_vulkan.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_descriptor_set_layouts_construct(VoidPtr *out_context);
VYTAL_API RendererBackendResult renderer_backend_vulkan_descriptor_set_layouts_destruct(VoidPtr *out_context);
