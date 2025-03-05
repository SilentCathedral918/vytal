#pragma once

#include "vytal/defines/assets/mesh.h"
#include "vytal/defines/shared.h"

VYTAL_API MeshResult mesh_loader_gltf_load_from_file(ConstStr filepath, Mesh *out_mesh);
VYTAL_API MeshResult mesh_loader_gltf_load_from_memory(const VoidPtr buffer, const ByteSize buffer_size, Mesh *out_mesh);
VYTAL_API MeshResult mesh_loader_gltf_unload(Mesh mesh);
