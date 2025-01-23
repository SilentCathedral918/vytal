#include "threading.h"

#include "vytal/core/hal/threading/core/sync/sync.h"
#include "vytal/core/hal/threading/core/task_queue/task_queue.h"
#include "vytal/core/hal/threading/module/thread.h"

Thread    *hal_threading_request_thread(void) { return thread_module_request_thread(); }
Bool       hal_threading_release_thread(Thread *thread) { return thread_module_release_thread(thread); }
TaskQueue *hal_threading_create_taskqueue(Mutex *mutex, CondVar *cond_var) { return hal_thread_taskqueue_construct(mutex, cond_var); }
Bool       hal_threading_destroy_taskqueue(TaskQueue *task_queue) { return hal_thread_taskqueue_destruct(task_queue); }
