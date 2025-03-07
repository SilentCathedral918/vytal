#pragma once

#include "vytal/defines/renderer/renderer.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_startup(const RendererBackendType backend_type, RendererBackend *out_backend);
VYTAL_API RendererBackendResult renderer_backend_shutdown(RendererBackend backend);
VYTAL_API RendererBackendResult renderer_backend_begin_frame(void);
VYTAL_API RendererBackendResult renderer_backend_end_frame(void);
VYTAL_API RendererBackendResult renderer_backend_render(void);
