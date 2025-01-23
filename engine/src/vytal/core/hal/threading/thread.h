#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API Thread *hal_thread_construct(VoidPtr func, VoidPtr args);
VT_API Bool    hal_thread_join(Thread *thread);
VT_API Bool    hal_thread_destruct(Thread *thread);