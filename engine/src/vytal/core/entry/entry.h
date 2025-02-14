#include "vytal/core/application/application.h"
#include "vytal/core/exception/exception.h"

Int32 main(Int32 argc, Str *argv) {
    AppResult app_result_;

    app_result_ = application_preconstruct();
    if (app_result_ != APP_SUCCESS)
        throw_exception(EXCEPTION_UNKNOWN, "Failed stage 'pre-construct' for application!");

    app_result_ = application_construct();
    if (app_result_ != APP_SUCCESS)
        throw_exception(EXCEPTION_UNKNOWN, "Failed stage 'construct' for application!");

    app_result_ = application_update();
    if (app_result_ != APP_SUCCESS)
        throw_exception(EXCEPTION_UNKNOWN, "Failed stage 'update' for application!");

    app_result_ = application_destruct();
    if (app_result_ != APP_SUCCESS)
        throw_exception(EXCEPTION_UNKNOWN, "Failed stage 'destruct' for application!");

    return 0;
}
