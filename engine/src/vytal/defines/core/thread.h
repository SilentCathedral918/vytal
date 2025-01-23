#pragma once

#include "container.h"
#include "types.h"

// ------------------------------- backends ------------------------------- //

typedef enum Thread_Backend { THREAD_BACKEND_POSIX, THREAD_BACKEND_WIN32 } ThreadBackend;

// -------------------------------- thread -------------------------------- //

typedef enum Thread_State { THREAD_STATE_CREATED, THREAD_STATE_RUNNING, THREAD_STATE_TERMINATED } ThreadState;

typedef struct Thread_Struct {
    UInt64      _id;
    VoidPtr     _handle;
    UInt32      _priority;
    ThreadState _state;
} Thread;

// -------------------------------- mutex -------------------------------- //

typedef struct Thread_Mutex {
    VoidPtr _handle;
} Mutex;

// ------------------------- condition variable ------------------------- //

typedef struct Thread_Conditional_Variable {
    VoidPtr _handle;
} CondVar;

// --------------------------- read-write lock --------------------------- //

typedef struct Thread_RWLock {
    VoidPtr _handle;
} RWLock;

// ----------------------------- task queue ----------------------------- //

typedef struct Thread_Task {
    UInt64 _id;
    void (*_func)(void *);
    VoidPtr _arg;
} Task;

typedef struct Thread_Task_Queue {
    Queue    _tasks;
    Mutex   *_mutex;
    CondVar *_cond_var;
} TaskQueue;
