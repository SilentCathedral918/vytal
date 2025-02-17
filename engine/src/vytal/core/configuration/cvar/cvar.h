#pragma once

#include "vytal/defines/core/cvar.h"
#include "vytal/defines/shared.h"

VYTAL_API CVarResult cvar_startup(void);
VYTAL_API CVarResult cvar_shutdown(void);

VYTAL_API CVarResult cvar_register(ConstStr name, CVarType type, CVarData default_value, Bool persistent, ConstStr description);
VYTAL_API CVarResult cvar_get(ConstStr name, CVarData *out_value);
VYTAL_API CVarResult cvar_set(ConstStr name, CVarData new_value);

VYTAL_API CVarResult cvar_load_from_file(ConstStr filepath, ConstStr description);
VYTAL_API CVarResult cvar_save_to_file(ConstStr filepath);
