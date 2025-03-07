#pragma once

#include "vytal/defines/core/filesystem.h"
#include "vytal/defines/renderer/renderer.h"
#include "vytal/defines/shared.h"

VYTAL_API RendererModuleResult renderer_module_startup(File *file);
VYTAL_API RendererModuleResult renderer_module_shutdown(void);

VYTAL_API RendererBackend renderer_module_get_backend(void);
