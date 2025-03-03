#include "unicast.h"

#include <string.h>

#include "vytal/core/containers/array/array.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/memory/zone/memory_zone.h"

typedef struct Delegate_Unicast_State {
    Map      _delegate_map;
    Bool     _initialized;
    ByteSize _memory_size;
} UnicastDelegateState;

static UnicastDelegateState *state = NULL;

DelegateResult delegate_unicast_startup(void) {
    if (state) return DELEGATE_ERROR_ALREADY_INITIALIZED;

    ByteSize state_memory_size_ = 0;
    if (memory_zone_allocate("delegates", sizeof(UnicastDelegateState), (VoidPtr)&state, &state_memory_size_) != MEMORY_ZONE_SUCCESS)
        return DELEGATE_ERROR_ALLOCATION_FAILED;
    memset(state, 0, sizeof(UnicastDelegateState));

    // configure state members
    {
        if (container_map_construct(sizeof(struct Delegate_Unicast_Handle), &state->_delegate_map) != CONTAINER_SUCCESS) {
            if (memory_zone_deallocate("delegates", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            return DELEGATE_ERROR_ALLOCATION_FAILED;
        }

        state->_memory_size = state_memory_size_;
        state->_initialized = true;
    }

    return DELEGATE_SUCCESS;
}

DelegateResult delegate_unicast_shutdown(void) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;

    if (container_map_destruct(state->_delegate_map) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DEALLOCATION_FAILED;

    if (memory_zone_deallocate("delegates", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
        return DELEGATE_ERROR_DEALLOCATION_FAILED;

    state = NULL;
    return DELEGATE_SUCCESS;
}

DelegateResult delegate_unicast_bind(ConstStr delegate_id, VoidPtr listener, DelegateFunction callback) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!delegate_id || !callback) return DELEGATE_ERROR_INVALID_PARAM;

    UnicastDelegate del_ = NULL;

    // search for delegate
    if (container_map_search(state->_delegate_map, delegate_id, (VoidPtr *)&del_) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DATA_SEARCH_FAILED;

    // if delegate is already registered, exit early
    if (del_ != NULL)
        return DELEGATE_SUCCESS;

    // allocate delegate
    if (memory_zone_allocate("delegates", sizeof(struct Delegate_Unicast_Handle), (VoidPtr *)&del_, NULL) != MEMORY_ZONE_SUCCESS) return DELEGATE_ERROR_ALLOCATION_FAILED;

    // configure delegate
    {
        del_->_listener = listener;
        del_->_callback = callback;
    }

    // register delegate
    if (container_map_insert(&state->_delegate_map, delegate_id, del_) != CONTAINER_SUCCESS) {
        if (memory_zone_deallocate("delegates", del_, sizeof(struct Delegate_Unicast_Handle)) != MEMORY_ZONE_SUCCESS)
            return DELEGATE_ERROR_DEALLOCATION_FAILED;

        return DELEGATE_ERROR_DATA_INSERT_FAILED;
    }

    return DELEGATE_SUCCESS;
}

DelegateResult delegate_unicast_unbind(ConstStr delegate_id) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!delegate_id) return DELEGATE_ERROR_INVALID_PARAM;

    // deallocate delegate
    {
        UnicastDelegate del_ = NULL;
        if (container_map_search(state->_delegate_map, delegate_id, (VoidPtr *)&del_) != CONTAINER_SUCCESS)
            return DELEGATE_ERROR_DATA_SEARCH_FAILED;
        if (!del_) return DELEGATE_ERROR_DATA_NOT_EXIST;

        if (memory_zone_deallocate("delegates", del_, sizeof(struct Delegate_Unicast_Handle)) != MEMORY_ZONE_SUCCESS)
            return DELEGATE_ERROR_DEALLOCATION_FAILED;
    }

    // remove its reference from the map
    if (container_map_remove(&state->_delegate_map, delegate_id) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DATA_REMOVE_FAILED;

    return DELEGATE_SUCCESS;
}

DelegateResult delegate_unicast_set_callback(ConstStr delegate_id, DelegateFunction callback) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!delegate_id || !callback) return DELEGATE_ERROR_INVALID_PARAM;

    UnicastDelegate del_ = NULL;
    if (container_map_search(state->_delegate_map, delegate_id, (VoidPtr *)&del_) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DATA_SEARCH_FAILED;
    if (!del_) return DELEGATE_ERROR_DATA_NOT_EXIST;

    del_->_callback = callback;
    return DELEGATE_SUCCESS;
}

DelegateResult delegate_unicast_invoke(ConstStr delegate_id, VoidPtr sender, VoidPtr data) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!delegate_id || !data) return DELEGATE_ERROR_INVALID_PARAM;

    UnicastDelegate del_ = NULL;
    if (container_map_search(state->_delegate_map, delegate_id, (VoidPtr *)&del_) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DATA_SEARCH_FAILED;
    if (!del_) return DELEGATE_ERROR_DATA_NOT_EXIST;

    if (!del_->_callback)
        return DELEGATE_ERROR_INVALID_CALLBACK;

    del_->_callback(sender, del_->_listener, data);
    return DELEGATE_SUCCESS;
}
