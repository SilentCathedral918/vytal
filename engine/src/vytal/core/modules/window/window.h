#pragma once

#include "vytal/defines/core/types.h"
#include "vytal/defines/core/window.h"
#include "vytal/defines/shared.h"

ByteSize window_module_get_size(void);

Bool           window_module_startup(VoidPtr module);
Bool           window_module_shutdown(void);
Bool           window_module_construct_main(void);
Bool           window_module_destruct_main(void);
PlatformWindow window_module_get_main(void);