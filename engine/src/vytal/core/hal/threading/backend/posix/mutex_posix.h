#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API Mutex *hal_mutex_posix_construct(void);
VT_API Bool   hal_mutex_posix_destruct(Mutex *mutex);
VT_API Bool   hal_mutex_posix_lock(Mutex *mutex);
VT_API Bool   hal_mutex_posix_trylock(Mutex *mutex);
VT_API Bool   hal_mutex_posix_unlock(Mutex *mutex);
