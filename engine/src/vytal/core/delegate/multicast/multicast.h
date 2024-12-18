#pragma once

#include "vytal/defines/core/delegate.h"
#include "vytal/defines/shared.h"

// -------------------------- delegate -------------------------- //

VT_API MulticastDelegate delegate_multicast_construct(VoidPtr listener);
VT_API Bool              delegate_multicast_destruct(MulticastDelegate delegate);
VT_API Bool              delegate_multicast_add_callback(MulticastDelegate delegate, DelegateFunc callback);
VT_API Bool              delegate_multicast_remove_callback(MulticastDelegate delegate, DelegateFunc callback);
VT_API Bool              delegate_multicast_clear_callbacks(MulticastDelegate delegate);

// ---------------------- delegates handle---------------------- //

VT_API MulticastDelegateHandle delegate_multicast_handle_construct(void);
VT_API Bool                    delegate_multicast_handle_destruct(MulticastDelegateHandle handle);
VT_API Bool                    delegate_multicast_handle_bind(MulticastDelegateHandle handle, MulticastDelegate delegate);
VT_API Bool                    delegate_multicast_handle_unbind(MulticastDelegateHandle handle, MulticastDelegate delegate);
VT_API Bool                    delegate_multicast_handle_invoke(MulticastDelegateHandle handle, VoidPtr sender, VoidPtr data);
