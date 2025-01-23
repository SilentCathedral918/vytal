#include "task_queue.h"

#include "vytal/core/containers/queue/queue.h"
#include "vytal/core/hal/threading/core/sync/sync.h"
#include "vytal/managers/memory/memmgr.h"

TaskQueue *hal_thread_taskqueue_construct(Mutex *mutex, CondVar *cond_var) {
    TaskQueue *tq_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!tq_)
        return NULL;

    tq_->_tasks = container_queue_construct();
    if (!tq_->_tasks) {
        memory_manager_deallocate(tq_, MEMORY_TAG_CONTAINERS);
        return NULL;
    }

    tq_->_mutex    = mutex;
    tq_->_cond_var = cond_var;
    return tq_;
}
Bool hal_thread_taskqueue_destruct(TaskQueue *task_queue) { return !task_queue ? false : container_queue_destruct(task_queue->_tasks) && memory_manager_deallocate(task_queue, MEMORY_TAG_THREADING); }
Bool hal_thread_taskqueue_enqueue(TaskQueue *task_queue, Task *task) {
    if (!task_queue || !task)
        return false;

    // lock the mutex before modifying the queue
    if (!hal_mutex_lock(task_queue->_mutex))
        return false;

    // enqueue new task into the queue
    if (!container_queue_enqueue(task_queue->_tasks, task, sizeof(Task))) {
        hal_mutex_unlock(task_queue->_mutex);
        return false; // failed to enqueue task
    }

    // signal the worker threads that a new task is available
    if (!hal_condvar_signal(task_queue->_cond_var))
        return false;

    // unlock the mutex when modification finished
    if (!hal_mutex_unlock(task_queue->_mutex))
        return false;

    return true;
}
Bool hal_thread_taskqueue_dequeue(TaskQueue *task_queue, Task *task) {
    if (!task_queue || !task)
        return false;

    // lock the mutex before modifying the queue
    if (!hal_mutex_lock(task_queue->_mutex))
        return false;

    // wait for available task if task queue is empty
    while (container_queue_is_empty(task_queue->_tasks)) {
        if (!hal_condvar_wait(task_queue->_cond_var, task_queue->_mutex))
            return false;
    }

    // dequeue the task from the queue
    if (!container_queue_dequeue(task_queue->_tasks, task)) {
        hal_mutex_unlock(task_queue->_mutex);
        return false; // failed to dequeue task
    }

    // unlock the mutex when modification finished
    if (!hal_mutex_unlock(task_queue->_mutex))
        return false;

    return true;
}
Queue hal_thread_taskqueue_get_queue(TaskQueue *task_queue) { return !task_queue ? NULL : task_queue->_tasks; }
