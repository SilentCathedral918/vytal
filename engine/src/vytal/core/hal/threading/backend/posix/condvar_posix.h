#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API CondVar *hal_condvar_posix_construct(void);
VT_API Bool     hal_condvar_posix_destruct(CondVar *cv);
VT_API Bool     hal_condvar_posix_wait(CondVar *cv, Mutex *mutex);
VT_API Bool     hal_condvar_posix_wait_timed(CondVar *cv, Mutex *mutex, const UInt32 timeout_ms);
VT_API Bool     hal_condvar_posix_signal(CondVar *cv);
VT_API Bool     hal_condvar_posix_broadcast(CondVar *cv);
