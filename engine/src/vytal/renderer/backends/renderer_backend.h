#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/renderer.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererBackendResult renderer_backend_startup(const RendererBackendType backend_type, Window *out_first_window, RendererBackend *out_backend);
VYTAL_API RendererBackendResult renderer_backend_shutdown(RendererBackend backend);
VYTAL_API RendererBackendResult renderer_backend_begin_frame(void);
VYTAL_API RendererBackendResult renderer_backend_end_frame(void);
VYTAL_API RendererBackendResult renderer_backend_render(void);

VYTAL_API RendererBackendResult renderer_backend_register_window(RendererBackend backend, Window *out_window);
VYTAL_API RendererBackendResult renderer_backend_unregister_window(RendererBackend backend, Window *out_window);
