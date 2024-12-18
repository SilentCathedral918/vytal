#pragma once

#include "vytal/defines/core/delegate.h"
#include "vytal/defines/shared.h"

// -------------------------- delegate -------------------------- //

VT_API UnicastDelegate delegate_unicast_construct(VoidPtr listener, DelegateFunc callback);
VT_API Bool            delegate_unicast_destruct(UnicastDelegate delegate);

// ---------------------- delegates handle---------------------- //

VT_API UnicastDelegateHandle delegate_unicast_handle_construct(void);
VT_API Bool                  delegate_unicast_handle_destruct(UnicastDelegateHandle handle);
VT_API Bool                  delegate_unicast_handle_bind(UnicastDelegateHandle handle, UnicastDelegate delegate);
VT_API Bool                  delegate_unicast_handle_unbind(UnicastDelegateHandle handle, UnicastDelegate delegate);
VT_API Bool                  delegate_unicast_handle_invoke(UnicastDelegateHandle handle, VoidPtr sender, VoidPtr data);
