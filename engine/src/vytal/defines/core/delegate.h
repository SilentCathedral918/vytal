#pragma once

#include "container.h"
#include "types.h"

typedef void (*Delegate_Function)(VoidPtr sender, VoidPtr listener, VoidPtr data);
typedef Delegate_Function DelegateFunc;

// -------------------------------- unicast -------------------------------- //

typedef struct Delegate_Unicast {
    VoidPtr      _listener;
    DelegateFunc _callback;
} Delegate_Unicast;
typedef Delegate_Unicast *UnicastDelegate;

typedef struct Delegate_Unicast_Handle {
    Map   _delegate_map;
    Array _binded_refs;
} Delegate_Unicast_Handle;
typedef Delegate_Unicast_Handle *UnicastDelegateHandle;

// -------------------------------- multicast -------------------------------- //

typedef struct Delegate_Multicast {
    VoidPtr _listener;
    Array   _callbacks;
    UInt32  _num_callbacks;
} Delegate_Multicast;
typedef Delegate_Multicast *MulticastDelegate;

typedef struct Delegate_Multicast_Handle {
    Map   _delegate_map;
    Array _binded_refs;
} Delegate_Multicast_Handle;
typedef Delegate_Multicast_Handle *MulticastDelegateHandle;
