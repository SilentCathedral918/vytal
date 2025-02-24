#pragma once

#include "vytal/defines/core/delegates.h"
#include "vytal/defines/shared.h"

VYTAL_API DelegateResult delegate_multicast_startup(void);
VYTAL_API DelegateResult delegate_multicast_shutdown(void);

VYTAL_API DelegateResult delegate_multicast_add(ConstStr delegate_id, VoidPtr listener, DelegateFunction callback);
VYTAL_API DelegateResult delegate_multicast_remove(ConstStr delegate_id, DelegateFunction callback, const Bool remove_all);
VYTAL_API DelegateResult delegate_multicast_invoke(ConstStr delegate_id, VoidPtr sender, VoidPtr data);
