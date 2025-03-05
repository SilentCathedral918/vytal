#pragma once

#include "vytal/defines/assets/mesh.h"
#include "vytal/defines/shared.h"

VYTAL_API MeshResult mesh_loader_load_from_file(ConstStr filepath, Mesh *out_mesh);
VYTAL_API MeshResult mesh_loader_load_from_memory(const VoidPtr buffer, const ByteSize buffer_size, const MeshFormat format, Mesh *out_mesh);
VYTAL_API MeshResult mesh_loader_unload(Mesh mesh);
