#include "delay.h"

#if defined(_MCS_VER)
#    include <windows.h>
#    define hal_sleep(ms) Sleep(ms)

#elif defined(__GNUC__) || defined(__clang__)
#    include <unistd.h>
#    define hal_sleep(ms) usleep(ms * 1000)

#endif

void hal_delay(UInt64 ms) { hal_sleep(ms); }
