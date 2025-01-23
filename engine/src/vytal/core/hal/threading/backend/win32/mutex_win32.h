#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API Mutex *hal_mutex_win32_construct(void);
VT_API Bool   hal_mutex_win32_destruct(Mutex *mutex);
VT_API Bool   hal_mutex_win32_lock(Mutex *mutex);
VT_API Bool   hal_mutex_win32_trylock(Mutex *mutex);
VT_API Bool   hal_mutex_win32_unlock(Mutex *mutex);
