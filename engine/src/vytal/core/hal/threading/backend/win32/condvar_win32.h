#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API CondVar *hal_condvar_win32_construct(void);
VT_API Bool     hal_condvar_win32_destruct(CondVar *cv);
VT_API Bool     hal_condvar_win32_wait(CondVar *cv, Mutex *mutex);
VT_API Bool     hal_condvar_win32_wait_timed(CondVar *cv, Mutex *mutex, const UInt32 timeout_ms);
VT_API Bool     hal_condvar_win32_signal(CondVar *cv);
VT_API Bool     hal_condvar_win32_broadcast(CondVar *cv);
