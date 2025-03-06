#pragma once

#include "vytal/defines/assets/mesh.h"
#include "vytal/defines/shared.h"

VYTAL_API MeshResult mesh_load_from_file(ConstStr name, ConstStr filepath, Mesh *out_mesh);
VYTAL_API MeshResult mesh_load_from_memory(ConstStr name, const VoidPtr buffer, const ByteSize buffer_size, const MeshFormat format, Mesh *out_mesh);
VYTAL_API MeshResult mesh_unload(ConstStr name);
