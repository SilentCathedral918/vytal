#pragma once

#include "vytal/defines/assets/image.h"
#include "vytal/defines/shared.h"

static VT_VAR_NOT_USED struct Asset_Manager_Struct {
    VoidPtr _internal_state;
} asset_manager;

VT_API void asset_manager_startup(void);
VT_API void asset_manager_shutdown(void);
