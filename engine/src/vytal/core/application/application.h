#pragma once

#include "vytal/defines/core/types.h"
#include "vytal/defines/shared.h"

VT_API Bool application_preconstruct(void);
VT_API Bool application_construct(void);
VT_API Bool application_update(void);
VT_API Bool application_destruct(void);
