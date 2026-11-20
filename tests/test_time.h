#ifndef TEST_TIME_H
#define TEST_TIME_H

#include <stdint.h>

void TestTime_SetMilliseconds(uint64_t time_ms);
void TestTime_AdvanceMilliseconds(uint64_t delta_ms);

#endif
