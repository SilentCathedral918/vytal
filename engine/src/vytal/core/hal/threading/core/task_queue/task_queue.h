#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API TaskQueue *hal_thread_taskqueue_construct(Mutex *mutex, CondVar *cond_var);
VT_API Bool       hal_thread_taskqueue_destruct(TaskQueue *task_queue);
VT_API Bool       hal_thread_taskqueue_enqueue(TaskQueue *task_queue, Task *task);
VT_API Bool       hal_thread_taskqueue_dequeue(TaskQueue *task_queue, Task *task);
VT_API Queue      hal_thread_taskqueue_get_queue(TaskQueue *task_queue);
