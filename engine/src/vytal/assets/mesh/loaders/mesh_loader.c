#include "mesh_loader.h"

#include <string.h>

#include "vytal/assets/mesh/loaders/gltf/loader_gltf.h"

MeshResult mesh_loader_load_from_file(ConstStr filepath, Mesh *out_mesh) {
    if (!filepath || !out_mesh) return MESH_ERROR_INVALID_PARAM;

    ConstStr ext_sep_ = strrchr(filepath, '.');
    if (!ext_sep_ || !*(ext_sep_ + 1)) return MESH_ERROR_LOAD_FAILED;
    ConstStr ext_ = ext_sep_ + 1;

    if (!strcasecmp(ext_, "glb") || !strcasecmp(ext_, "gltf"))
        return mesh_loader_gltf_load_from_file(filepath, out_mesh);

    else if (!strcasecmp(ext_, "fbx"))
        return MESH_SUCCESS;

    else if (!strcasecmp(ext_, "obj"))
        return MESH_SUCCESS;

    return MESH_ERROR_LOAD_FAILED;
}

MeshResult mesh_loader_load_from_memory(const VoidPtr buffer, const ByteSize buffer_size, const MeshFormat format, Mesh *out_mesh) {
    if (!buffer || !buffer_size || !out_mesh || (format < 0) || (format > MESH_FORMAT_FBX)) return MESH_ERROR_INVALID_PARAM;

    switch (format) {
        case MESH_FORMAT_GLTF:
        case MESH_FORMAT_GLB:
            return mesh_loader_gltf_load_from_memory(buffer, buffer_size, out_mesh);

        case MESH_FORMAT_OBJ:
        case MESH_FORMAT_FBX:
            return MESH_SUCCESS;

        default:
            return MESH_ERROR_LOAD_FAILED;
    }
}

MeshResult mesh_loader_unload(Mesh mesh) {
    if (!mesh) return MESH_ERROR_INVALID_PARAM;

    switch (mesh->_format) {
        case MESH_FORMAT_GLTF:
        case MESH_FORMAT_GLB:
            return mesh_loader_gltf_unload(mesh);

        case MESH_FORMAT_OBJ:
        case MESH_FORMAT_FBX:
            return MESH_SUCCESS;

        default:
            return MESH_ERROR_UNLOAD_FAILED;
    }
}
