#pragma once

#include "vytal/defines/renderer/backends/backend_vulkan.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_instance_construct(VoidPtr *out_context);
VYTAL_API RendererBackendResult renderer_backend_vulkan_instance_destruct(VoidPtr *out_context);
