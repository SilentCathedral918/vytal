#include "mesh.h"

#include "vytal/assets/mesh/loaders/mesh_loader.h"
#include "vytal/assets/mesh/module/mesh_module.h"

MeshResult mesh_load_from_file(ConstStr name, ConstStr filepath, Mesh *out_mesh) {
    if (!name || !filepath || !out_mesh) return MESH_ERROR_INVALID_PARAM;

    // if mesh already registered, unload the current one
    Mesh loaded_mesh_ = mesh_module_get(name);
    if (loaded_mesh_) {
        if (mesh_unload(name) != MESH_SUCCESS)
            return MESH_ERROR_LOAD_FAILED;
    }

    MeshResult load_mesh_ = mesh_loader_load_from_file(filepath, out_mesh);
    if (load_mesh_ != MESH_SUCCESS)
        return load_mesh_;

    if (mesh_module_register(name, (*out_mesh)) != MESH_MODULE_SUCCESS)
        return MESH_ERROR_LOAD_FAILED;

    return MESH_SUCCESS;
}

MeshResult mesh_load_from_memory(ConstStr name, VoidPtr buffer, ByteSize buffer_size, MeshFormat format, Mesh *out_mesh) {
    if (!name || !buffer || !buffer_size || !out_mesh) return MESH_ERROR_INVALID_PARAM;

    // if mesh already registered, unload the current one
    Mesh loaded_mesh_ = mesh_module_get(name);
    if (loaded_mesh_) {
        if (mesh_unload(name) != MESH_SUCCESS)
            return MESH_ERROR_LOAD_FAILED;
    }

    MeshResult load_mesh_ = mesh_loader_load_from_memory(buffer, buffer_size, format, out_mesh);
    if (load_mesh_ != MESH_SUCCESS)
        return load_mesh_;

    if (mesh_module_register(name, (*out_mesh)) != MESH_MODULE_SUCCESS)
        return MESH_ERROR_LOAD_FAILED;

    return MESH_SUCCESS;
}

MeshResult mesh_unload(ConstStr name) {
    if (!name) return MESH_ERROR_INVALID_PARAM;

    Mesh mesh_ = mesh_module_get(name);
    if (!mesh_) return MESH_ERROR_UNLOAD_FAILED;

    if (mesh_module_unregister(name) != MESH_MODULE_SUCCESS)
        return MESH_ERROR_UNLOAD_FAILED;

    MeshResult unload_mesh_ = mesh_loader_unload(mesh_);
    if (unload_mesh_ != MESH_SUCCESS)
        return unload_mesh_;

    return MESH_SUCCESS;
}
