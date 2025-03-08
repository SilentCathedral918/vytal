#pragma once

#include "vytal/defines/core/filesystem.h"
#include "vytal/defines/core/window.h"
#include "vytal/defines/renderer/renderer.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererModuleResult renderer_module_startup(File *file, Window *out_first_window);
VYTAL_API RendererModuleResult renderer_module_shutdown(void);

VYTAL_API RendererModuleResult renderer_module_register_window(Window *out_window);
VYTAL_API RendererModuleResult renderer_module_unregister_window(Window *out_window);

VYTAL_API RendererBackend renderer_module_get_backend(void);