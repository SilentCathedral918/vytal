#include "unicast.h"

#include "vytal/core/containers/array/array.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/managers/memory/memmgr.h"

// -------------------------- delegate -------------------------- //

UnicastDelegate delegate_unicast_construct(VoidPtr listener, DelegateFunc callback) {
    if (!listener || !callback)
        return NULL;

    // allocate the delegate
    UnicastDelegate del_ = memory_manager_allocate(0, MEMORY_TAG_DELEGATES);

    // init delegate members
    {
        del_->_listener = listener;
        del_->_callback = callback;
    }

    return del_;
}

Bool delegate_unicast_destruct(UnicastDelegate delegate) {
    if (!delegate)
        return false;

    // set members to zero
    hal_mem_memzero(delegate, sizeof(Delegate_Unicast));

    // free the delegate self
    if (!memory_manager_deallocate(delegate, MEMORY_TAG_DELEGATES))
        return false;

    return true;
}

Bool delegate_unicast_set_callback(UnicastDelegate delegate, DelegateFunc callback) {
    if (!delegate || !callback)
        return false;

    delegate->_callback = callback;
    return true;
}

// ---------------------- delegates handle ---------------------- //

UnicastDelegateHandle delegate_unicast_handle_construct(void) {
    // allocate the handle
    UnicastDelegateHandle handle_ = memory_manager_allocate(0, MEMORY_TAG_DELEGATES);

    // init handle members
    {
        handle_->_delegate_map = container_map_construct(sizeof(UnicastDelegate), NULL);
        handle_->_binded_refs  = container_array_construct(UIntPtr);
    }

    return handle_;
}

Bool delegate_unicast_handle_destruct(UnicastDelegateHandle handle) {
    if (!handle)
        return false;

    // free and set members to zero
    {
        container_array_destruct(handle->_binded_refs);
        container_map_destruct(handle->_delegate_map);
        hal_mem_memzero(handle, sizeof(Delegate_Unicast_Handle));
    }

    // free the handle self
    if (!memory_manager_deallocate(handle, MEMORY_TAG_DELEGATES))
        return false;

    return true;
}

Bool delegate_unicast_handle_bind(UnicastDelegateHandle handle, UnicastDelegate delegate) {
    if (!handle || !delegate)
        return false;

    // check if delegate is already binded to the handle
    if (container_map_contains(handle->_delegate_map, VT_CAST(ConstStr, delegate)))
        return false;

    // bind new delegate
    {
        if (!container_map_insert(handle->_delegate_map, VT_CAST(ConstStr, delegate), &delegate))
            return false;

        container_array_push(handle->_binded_refs, UIntPtr, VT_CAST(UIntPtr, delegate));
    }

    return true;
}

Bool delegate_unicast_handle_unbind(UnicastDelegateHandle handle, UnicastDelegate delegate) {
    if (!handle || !delegate)
        return false;

    // check if delegate is not binded to the handle
    if (!container_map_contains(handle->_delegate_map, VT_CAST(ConstStr, delegate)))
        return false;

    // unbind the delegate
    {
        if (!container_map_remove(handle->_delegate_map, VT_CAST(ConstStr, delegate)))
            return false;

        UIntPtr remove_data_ = VT_CAST(UIntPtr, delegate);
        container_array_remove(handle->_binded_refs, &remove_data_);
    }

    return true;
}

Bool delegate_unicast_handle_invoke(UnicastDelegateHandle handle, VoidPtr sender, VoidPtr data) {
    if (!handle || !data)
        return false;

    // go through the delegates...
    for (ByteSize i = 0; i < container_array_length(handle->_binded_refs); ++i) {
        UIntPtr         ref_ = container_array_get_value_at_index(handle->_binded_refs, UIntPtr, i);
        UnicastDelegate del_ = VT_CAST(UnicastDelegate, ref_);

        if (!del_)
            continue;

        del_->_callback(sender, del_->_listener, data);
        return true;
    }

    return false;
}
