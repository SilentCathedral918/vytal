#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/shared.h"

VYTAL_API WindowResult window_startup(const WindowBackend backend);
VYTAL_API WindowResult window_shutdown(void);
