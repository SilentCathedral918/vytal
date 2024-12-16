#pragma once

#include "vytal/core/application/application.h"
#include "vytal/core/misc/assertions/assertions.h"

Int32 main(Int32 argc, Str *argv) {
    VT_ASSERT_MESSAGE(application_preconstruct(), "pre_construct stage failed!");
    VT_ASSERT_MESSAGE(application_construct(), "construct stage failed!");
    VT_ASSERT_MESSAGE(application_update(), "game loop failed!");
    VT_ASSERT_MESSAGE(application_destruct(), "cleanup failed!");

    return 0;
}
