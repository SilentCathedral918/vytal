#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API ByteSize thread_module_get_size(void);

VT_API Bool          thread_module_startup(VoidPtr module, const ThreadBackend backend);
VT_API Bool          thread_module_shutdown(void);
VT_API Thread       *thread_module_request_thread(void);
VT_API Bool          thread_module_release_thread(Thread *thread);
VT_API VoidPtr       thread_module_get_state(void);
VT_API ThreadBackend thread_module_get_backend(void);
VT_API ByteSize      thread_module_get_thread_count(void);
VT_API void          thread_module_switch_backend(const ThreadBackend backend);
