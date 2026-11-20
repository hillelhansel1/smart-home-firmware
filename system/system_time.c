#include "system_time.h"
#include <windows.h>

uint64_t Time_GetMilliseconds(void) {
    return (uint64_t)GetTickCount64();
}