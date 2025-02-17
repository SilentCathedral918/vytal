#include "exception.h"

#if defined(_MSC_VER)

void exception_startup(void) {
    exception_stack_key     = TlsAlloc();
    exception_stack_top_key = TlsAlloc();
}

void exception_shutdown(void) {
    TlsFree(exception_stack_key);
    TlsFree(exception_stack_top_key);
}

ExceptionFrame *exception_get_stack(void) {
    ExceptionFrame *stack_ = TlsGetValue(exception_stack_key);

    if (!stack_) {
        stack_ = calloc(MAX_EXCEPTION_DEPTH, sizeof(ExceptionFrame));
        TlsSetValue(exception_stack_key, stack_);
    }

    return stack_;
}

Int32 *exception_get_stack_top(void) {
    Int32 *top_ = TlsGetValue(exception_stack_top_key);

    if (!top_) {
        top_  = calloc(1, sizeof(Int32));
        *top_ = -1;

        TlsSetValue(exception_stack_top_key, top_);
    }

    return top_;
}

#elif defined(__GNUC__) || defined(__clang__)

void exception_startup(void) {
    pthread_key_create(&exception_stack_key, free);
    pthread_key_create(&exception_stack_top_key, free);
}

void exception_shutdown(void) {
    pthread_key_delete(exception_stack_key);
    pthread_key_delete(exception_stack_top_key);
}

ExceptionFrame *exception_get_stack(void) {
    ExceptionFrame *stack_ = pthread_getspecific(exception_stack_key);

    if (!stack_) {
        stack_ = calloc(MAX_EXCEPTION_DEPTH, sizeof(ExceptionFrame));
        pthread_setspecific(exception_stack_key, stack_);
    }

    return stack_;
}

Int32 *exception_get_stack_top(void) {
    Int32 *top_ = pthread_getspecific(exception_stack_top_key);

    if (!top_) {
        top_  = calloc(1, sizeof(Int32));
        *top_ = -1;

        pthread_setspecific(exception_stack_top_key, top_);
    }

    return top_;
}

#endif
