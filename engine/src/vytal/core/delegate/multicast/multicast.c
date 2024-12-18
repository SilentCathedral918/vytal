#include "multicast.h"

#include "vytal/core/containers/array/array.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/managers/memory/memmgr.h"

// -------------------------- delegate -------------------------- //

MulticastDelegate delegate_multicast_construct(VoidPtr listener) {
    if (!listener)
        return NULL;

    // allocate the delegate
    MulticastDelegate del_ = memory_manager_allocate(sizeof(Delegate_Multicast), MEMORY_TAG_DELEGATES);

    // init delegate members
    {
        del_->_listener      = listener;
        del_->_num_callbacks = 0;
        del_->_callbacks     = container_array_construct(DelegateFunc);
    }

    return del_;
}

Bool delegate_multicast_destruct(MulticastDelegate delegate) {
    if (!delegate)
        return false;

    // free and set members to zero
    {
        if (!container_array_destruct(delegate->_callbacks))
            return false;

        hal_mem_memzero(delegate, sizeof(Delegate_Unicast));
    }

    // free the delegate self
    if (!memory_manager_deallocate(delegate, MEMORY_TAG_DELEGATES))
        return false;

    return true;
}

Bool delegate_multicast_add_callback(MulticastDelegate delegate, DelegateFunc callback) {
    if (!delegate || !callback)
        return false;

    container_array_push(delegate->_callbacks, DelegateFunc, callback);
    ++delegate->_num_callbacks;

    return true;
}

Bool delegate_multicast_remove_callback(MulticastDelegate delegate, DelegateFunc callback) {
    if (!delegate || !callback)
        return false;

    if (!container_array_remove(delegate->_callbacks, callback))
        return false;
    --delegate->_num_callbacks;

    return true;
}

Bool delegate_multicast_clear_callbacks(MulticastDelegate delegate) {
    if (!delegate)
        return false;

    if (!container_array_clear(delegate->_callbacks))
        return false;
    delegate->_num_callbacks = 0;

    return true;
}

// ---------------------- delegates handle---------------------- //

MulticastDelegateHandle delegate_multicast_handle_construct(void) {
    // allocate the handle
    MulticastDelegateHandle handle_ = memory_manager_allocate(0, MEMORY_TAG_DELEGATES);

    // init handle members
    {
        handle_->_delegate_map  = container_map_construct(sizeof(MulticastDelegate));
        handle_->_num_delegates = 0;
    }

    return handle_;
}

Bool delegate_multicast_handle_destruct(MulticastDelegateHandle handle) {
    if (!handle)
        return false;

    // free and set members to zero
    {
        container_map_destruct(handle->_delegate_map);
        hal_mem_memzero(handle, sizeof(Delegate_Unicast_Handle));
    }

    // free the handle self
    if (!memory_manager_deallocate(handle, MEMORY_TAG_DELEGATES))
        return false;

    return true;
}

Bool delegate_multicast_handle_bind(MulticastDelegateHandle handle, MulticastDelegate delegate) {
    if (!handle || !delegate)
        return false;

    // check if delegate is already binded to the handle
    if (container_map_contains(handle->_delegate_map, delegate))
        return false;

    // bind new delegate
    {
        if (!container_map_insert(handle->_delegate_map, delegate, &delegate))
            return false;

        ++handle->_num_delegates;
    }

    return true;
}

Bool delegate_multicast_handle_unbind(MulticastDelegateHandle handle, MulticastDelegate delegate) {
    if (!handle || !delegate)
        return false;

    // check if delegate is not binded to the handle
    if (!container_map_contains(handle->_delegate_map, delegate))
        return false;

    // unbind the delegate
    {
        if (!container_map_remove(handle->_delegate_map, delegate))
            return false;

        --handle->_num_delegates;
    }

    return true;
}

Bool delegate_multicast_handle_invoke(MulticastDelegateHandle handle, VoidPtr sender, VoidPtr data) {
    if (!handle || !sender || !data)
        return false;

    // go through the delegates...
    for (ByteSize i = 0; i < handle->_num_delegates; ++i) {
        MulticastDelegate del_ = container_map_get_value_by_index(handle->_delegate_map, MulticastDelegate, i);
        if (!del_)
            continue;

        // del_->_callback(sender, del_->_listener, data);
        // go through the callbacks...
        for (ByteSize j = 0; j < del_->_num_callbacks; ++j) {
            DelegateFunc callback_ = container_array_get_value_at_index(del_->_callbacks, DelegateFunc, j);

            // ... and invoke individually
            callback_(sender, del_->_listener, data);
        }
    }

    return true;
}
