#include "application.h"

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/exception/exception.h"
#include "vytal/core/platform/filesystem/filesystem.h"

AppResult application_preconstruct(void) {
    exception_startup();

    CVarResult cvar_startup_ = cvar_startup();
    if (cvar_startup_ != CVAR_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    return APP_SUCCESS;
}

AppResult application_construct(void) {
    return APP_SUCCESS;
}

AppResult application_update(void) {
    return APP_SUCCESS;
}

AppResult application_destruct(void) {
    CVarResult cvar_shutdown_ = cvar_shutdown();
    if (cvar_shutdown_ != CVAR_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    exception_shutdown();

    return APP_SUCCESS;
}
