#include "application.h"

#include "vytal/core/exception/exception.h"

AppResult application_preconstruct(void) {
    exception_startup();

    return APP_SUCCESS;
}

AppResult application_construct(void) {
    return APP_SUCCESS;
}

AppResult application_update(void) {
    return APP_SUCCESS;
}

AppResult application_destruct(void) {
    exception_shutdown();

    return APP_SUCCESS;
}
