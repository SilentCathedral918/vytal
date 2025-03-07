#pragma once

#include "vytal/defines/renderer/renderer.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_vulkan_instance_construct(const Bool validation_layer_enabled, VoidPtr out_debug_msg_info, VoidPtr *out_instance);
VYTAL_API RendererBackendResult renderer_backend_vulkan_instance_destruct(VoidPtr instance);
