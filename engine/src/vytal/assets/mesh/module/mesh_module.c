#include "mesh_module.h"

#include <string.h>

#include "vytal/assets/mesh/loaders/mesh_loader.h"
#include "vytal/core/containers/array/array.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/memory/zone/memory_zone.h"

typedef struct Mesh_Module_State {
    Map   _mesh_lookup;
    Array _registered_meshes;

    Bool     _initialized;
    ByteSize _memory_size;
} MeshModuleState;

static MeshModuleState *state = NULL;

MeshModuleResult mesh_module_startup(void) {
    if (state) return MESH_MODULE_ERROR_ALREADY_INITIALIZED;

    // allocate module state
    ByteSize alloc_size_ = 0;
    if (memory_zone_allocate("modules", sizeof(MeshModuleState), (VoidPtr *)&state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return MESH_MODULE_ERROR_ALLOCATION_FAILED;
    memset(state, 0, alloc_size_);

    // allocate and configure members
    {
        // mesh lookup
        if (container_map_construct(sizeof(Mesh), &state->_mesh_lookup) != CONTAINER_SUCCESS)
            return MESH_MODULE_ERROR_ALLOCATION_FAILED;

        // registered meshes
        if (container_array_construct(sizeof(Mesh), &state->_registered_meshes) != CONTAINER_SUCCESS)
            return MESH_MODULE_ERROR_ALLOCATION_FAILED;

        state->_initialized = true;
        state->_memory_size = alloc_size_;
    }

    return MESH_MODULE_SUCCESS;
}

MeshModuleResult mesh_module_shutdown(void) {
    if (!state || !state->_initialized) return MESH_MODULE_ERROR_NOT_INITIALIZED;

    // deallocate members
    {
        // registered meshes
        {
            for (ByteSize i = 0; i < container_array_size(state->_registered_meshes); ++i) {
                Mesh mesh_ = container_array_at_index(state->_registered_meshes, i);

                if (mesh_loader_unload(mesh_) != MESH_SUCCESS)
                    return MESH_MODULE_ERROR_DEALLOCATION_FAILED;
            }

            if (container_array_destruct(state->_registered_meshes) != CONTAINER_SUCCESS)
                return MESH_MODULE_ERROR_DEALLOCATION_FAILED;
            state->_registered_meshes = NULL;
        }

        // mesh lookup
        if (container_map_destruct(state->_mesh_lookup) != CONTAINER_SUCCESS)
            return MESH_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_mesh_lookup = NULL;
    }

    // deallocate module state
    {
        if (memory_zone_deallocate("modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return MESH_MODULE_ERROR_ALLOCATION_FAILED;
        state = NULL;
    }

    return MESH_MODULE_SUCCESS;
}

MeshModuleResult mesh_module_register(ConstStr name, Mesh mesh) {
    if (!state || !state->_initialized) return MESH_MODULE_ERROR_NOT_INITIALIZED;
    if (!name || !mesh) return MESH_MODULE_ERROR_INVALID_PARAM;

    // insert into lookup
    if (container_map_insert(&state->_mesh_lookup, name, (VoidPtr)&mesh) != CONTAINER_SUCCESS)
        return MESH_MODULE_ERROR_REGISTER_FAILED;

    // if successful, store reference into the array
    if (container_array_push(&state->_registered_meshes, (VoidPtr)&mesh) != CONTAINER_SUCCESS)
        return MESH_MODULE_ERROR_REGISTER_FAILED;

    return MESH_MODULE_SUCCESS;
}

MeshModuleResult mesh_module_unregister(ConstStr name) {
    if (!state || !state->_initialized) return MESH_MODULE_ERROR_NOT_INITIALIZED;
    if (!name) return MESH_MODULE_ERROR_INVALID_PARAM;

    Mesh mesh_ = NULL;
    if (container_map_search(state->_mesh_lookup, name, (VoidPtr)&mesh_) != CONTAINER_SUCCESS)
        return MESH_MODULE_ERROR_UNREGISTER_FAILED;

    // mesh is not registered
    if (!mesh_) return MESH_MODULE_ERROR_UNREGISTER_FAILED;

    // remove reference from array
    if (container_array_remove(&state->_registered_meshes, mesh_, false) != CONTAINER_SUCCESS)
        return MESH_MODULE_ERROR_UNREGISTER_FAILED;

    // remove from lookup
    if (container_map_remove(&state->_mesh_lookup, name) != CONTAINER_SUCCESS)
        return MESH_MODULE_ERROR_UNREGISTER_FAILED;

    return MESH_MODULE_SUCCESS;
}

Mesh mesh_module_get(ConstStr name) {
    if (!state || !state->_initialized) return NULL;
    if (!name) return NULL;

    Mesh mesh_ = NULL;
    if (container_map_search(state->_mesh_lookup, name, (VoidPtr)&mesh_) != CONTAINER_SUCCESS)
        return NULL;
    if (!mesh_) return NULL;

    return mesh_;
}
