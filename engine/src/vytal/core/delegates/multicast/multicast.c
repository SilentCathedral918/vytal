#include "multicast.h"

#include <string.h>

#include "vytal/core/containers/array/array.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/memory/zone/memory_zone.h"

typedef struct Delegate_Multicast_State {
    Map      _delegate_map;
    Array    _active_delegates;
    Bool     _initialized;
    ByteSize _memory_size;
} MulticastDelegateState;

static MulticastDelegateState *state = NULL;

DelegateResult delegate_multicast_startup(void) {
    if (state) return DELEGATE_ERROR_ALREADY_INITIALIZED;

    ByteSize state_memory_size_ = 0;
    if (memory_zone_allocate("delegates", sizeof(MulticastDelegateState), (VoidPtr *)&state, &state_memory_size_) != MEMORY_ZONE_SUCCESS)
        return DELEGATE_ERROR_ALLOCATION_FAILED;
    memset(state, 0, sizeof(MulticastDelegateState));

    // configure state members
    {
        if (container_map_construct(sizeof(struct Delegate_Multicast_Handle), &state->_delegate_map) != CONTAINER_SUCCESS) {
            if (memory_zone_deallocate("delegates", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            return DELEGATE_ERROR_ALLOCATION_FAILED;
        }

        if (container_array_construct(sizeof(struct Delegate_Multicast_Handle), &state->_active_delegates) != CONTAINER_SUCCESS) {
            if (container_map_destruct(state->_delegate_map) != CONTAINER_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            if (memory_zone_deallocate("delegates", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            return DELEGATE_ERROR_ALLOCATION_FAILED;
        }

        state->_memory_size = state_memory_size_;
        state->_initialized = true;
    }

    return DELEGATE_SUCCESS;
}

DelegateResult delegate_multicast_shutdown(void) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;

    for (ByteSize i = 0; i < container_array_size(state->_active_delegates); ++i) {
        MulticastDelegate del_ = container_array_at_index(state->_active_delegates, i);

        if (del_ && del_->_listener && del_->_callbacks) {
            if (container_array_destruct(del_->_callbacks) != CONTAINER_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            if (memory_zone_deallocate("delegates", del_, sizeof(struct Delegate_Multicast_Handle)) != MEMORY_ZONE_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;
        }
    }

    if (container_array_destruct(state->_active_delegates) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DEALLOCATION_FAILED;

    if (container_map_destruct(state->_delegate_map) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DEALLOCATION_FAILED;

    if (memory_zone_deallocate("delegates", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
        return DELEGATE_ERROR_DEALLOCATION_FAILED;

    state = NULL;
    return DELEGATE_SUCCESS;
}

DelegateResult delegate_multicast_add(ConstStr delegate_id, VoidPtr listener, DelegateFunction callback) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!delegate_id || !callback) return DELEGATE_ERROR_INVALID_PARAM;

    MulticastDelegate del_ = NULL;
    if (container_map_search(state->_delegate_map, delegate_id, (VoidPtr *)&del_) == CONTAINER_SUCCESS && del_) {
        // delegate already exists, simply push the callback to the list
        return (container_array_push(&del_->_callbacks, (VoidPtr)&callback) == CONTAINER_SUCCESS) ? DELEGATE_SUCCESS : DELEGATE_ERROR_DATA_INSERT_FAILED;
    }

    // otherwise
    {
        // allocate the delegate
        if (memory_zone_allocate("delegates", sizeof(struct Delegate_Multicast_Handle), (VoidPtr)&del_, NULL) != MEMORY_ZONE_SUCCESS)
            return DELEGATE_ERROR_ALLOCATION_FAILED;

        // bind the listener
        del_->_listener = listener;

        // construct a list of callbacks
        if (container_array_construct(sizeof(DelegateFunction), &del_->_callbacks) != CONTAINER_SUCCESS) {
            if (memory_zone_deallocate("delegates", del_, sizeof(struct Delegate_Multicast_Handle)) != MEMORY_ZONE_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            return DELEGATE_ERROR_ALLOCATION_FAILED;
        }

        // push the callback to the list
        if (container_array_push(&del_->_callbacks, (VoidPtr)&callback) != CONTAINER_SUCCESS) {
            if (container_array_destruct(del_->_callbacks) != CONTAINER_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            if (memory_zone_deallocate("delegates", del_, sizeof(struct Delegate_Multicast_Handle)) != MEMORY_ZONE_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            return DELEGATE_ERROR_DATA_INSERT_FAILED;
        }

        // insert the delegate into the map
        if (container_map_insert(&state->_delegate_map, delegate_id, (VoidPtr)del_) != CONTAINER_SUCCESS) {
            if (container_array_destruct(del_->_callbacks) != CONTAINER_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            if (memory_zone_deallocate("delegates", del_, sizeof(struct Delegate_Multicast_Handle)) != MEMORY_ZONE_SUCCESS)
                return DELEGATE_ERROR_DEALLOCATION_FAILED;

            return DELEGATE_ERROR_DATA_INSERT_FAILED;
        }

        // push the delegate reference to active_delegates
        if (container_array_push(&state->_active_delegates, (VoidPtr)&del_) != CONTAINER_SUCCESS) {
            if (container_map_remove(&state->_delegate_map, delegate_id) != CONTAINER_SUCCESS)
                return DELEGATE_ERROR_DATA_REMOVE_FAILED;

            return DELEGATE_ERROR_DATA_INSERT_FAILED;
        }
    }

    return DELEGATE_SUCCESS;
}

DelegateResult delegate_multicast_remove(ConstStr delegate_id, DelegateFunction callback, const Bool remove_all) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!delegate_id || !callback) return DELEGATE_ERROR_INVALID_PARAM;

    MulticastDelegate del_ = NULL;
    if (container_map_search(state->_delegate_map, delegate_id, (VoidPtr *)&del_) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DATA_SEARCH_FAILED;

    // if delegate is not found
    if (!del_) return DELEGATE_ERROR_DATA_NOT_EXIST;

    // remove matching callback(s)
    for (ByteSize i = 0; i < container_array_size(del_->_callbacks); ++i) {
        DelegateFunction *func_ = (DelegateFunction *)container_array_at_index(del_->_callbacks, i);
        if (func_ && ((*func_) == callback)) {
            container_array_remove(&del_->_callbacks, func_, remove_all);
            if (!remove_all) break;
        }
    }

    // if no callbacks remain, remove the delegate from the map
    if (container_array_size(del_->_callbacks) == 0) {
        if (container_array_destruct(del_->_callbacks) != CONTAINER_SUCCESS)
            return DELEGATE_ERROR_DEALLOCATION_FAILED;

        if (container_map_remove(&state->_delegate_map, delegate_id) != CONTAINER_SUCCESS)
            return DELEGATE_ERROR_DATA_REMOVE_FAILED;

        if (container_array_remove(&state->_active_delegates, del_, false) != CONTAINER_SUCCESS)
            return DELEGATE_ERROR_DATA_REMOVE_FAILED;

        if (memory_zone_deallocate("delegates", del_, sizeof(struct Delegate_Multicast_Handle)) != MEMORY_ZONE_SUCCESS)
            return DELEGATE_ERROR_DEALLOCATION_FAILED;

        del_ = NULL;
    }

    return DELEGATE_SUCCESS;
}

DelegateResult delegate_multicast_invoke(ConstStr delegate_id, VoidPtr sender, VoidPtr data) {
    if (!state) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return DELEGATE_ERROR_NOT_INITIALIZED;
    if (!delegate_id || !data) return DELEGATE_ERROR_INVALID_PARAM;

    MulticastDelegate del_ = NULL;
    if (container_map_search(state->_delegate_map, delegate_id, (VoidPtr *)&del_) != CONTAINER_SUCCESS)
        return DELEGATE_ERROR_DATA_SEARCH_FAILED;

    // if delegate is not found
    if (!del_) return DELEGATE_ERROR_DATA_NOT_EXIST;

    if (container_array_size(del_->_callbacks) > 0) {
        // go through every callback in the delegate list
        for (ByteSize i = 0; i < container_array_size(del_->_callbacks); ++i) {
            DelegateFunction *func_ = container_array_at_index(del_->_callbacks, i);

            // invoke the callback (if valid)
            if (*func_ && del_->_listener) (*func_)(sender, del_->_listener, data);
        }
    }

    return DELEGATE_SUCCESS;
}
