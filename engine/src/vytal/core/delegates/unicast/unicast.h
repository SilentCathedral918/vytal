#pragma once

#include "vytal/defines/core/delegates.h"
#include "vytal/defines/shared.h"

VYTAL_API DelegateResult delegate_unicast_startup(void);
VYTAL_API DelegateResult delegate_unicast_shutdown(void);

VYTAL_API DelegateResult delegate_unicast_bind(ConstStr delegate_id, VoidPtr listener, DelegateFunction callback);
VYTAL_API DelegateResult delegate_unicast_unbind(ConstStr delegate_id);
VYTAL_API DelegateResult delegate_unicast_set_callback(ConstStr delegate_id, DelegateFunction callback);
VYTAL_API DelegateResult delegate_unicast_invoke(ConstStr delegate_id, VoidPtr sender, VoidPtr data);
