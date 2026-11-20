#include "test_time.h"
#include "../system/system_time.h"

static uint64_t test_time_ms = 0;

uint64_t Time_GetMilliseconds(void)
{
    return test_time_ms;
}

void TestTime_SetMilliseconds(uint64_t time_ms)
{
    test_time_ms = time_ms;
}

void TestTime_AdvanceMilliseconds(uint64_t delta_ms)
{
    test_time_ms += delta_ms;
}
