#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API Thread    *hal_threading_request_thread(void);
VT_API Bool       hal_threading_release_thread(Thread *thread);
VT_API TaskQueue *hal_threading_create_taskqueue(Mutex *mutex, CondVar *cond_var);
VT_API Bool       hal_threading_destroy_taskqueue(TaskQueue *task_queue);
