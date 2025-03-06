#pragma once

#include "vytal/defines/assets/mesh.h"
#include "vytal/defines/shared.h"

VYTAL_API MeshModuleResult mesh_module_startup(void);
VYTAL_API MeshModuleResult mesh_module_shutdown(void);
VYTAL_API MeshModuleResult mesh_module_register(ConstStr name, Mesh mesh);
VYTAL_API MeshModuleResult mesh_module_unregister(ConstStr name);

VYTAL_API Mesh mesh_module_get(ConstStr name);
