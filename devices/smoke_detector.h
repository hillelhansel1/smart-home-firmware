#ifndef SMOKE_DETECTOR_H
#define SMOKE_DETECTOR_H

#include <stdbool.h>

typedef enum{
    SMOKE_DETECTED,
    SMOKE_NOT_DETECTED
}SmokeState;

typedef struct{
    int gpio_pin;
    volatile bool event_pending;
} SmokeDetector;

bool SmokeDetector_Init(SmokeDetector *detector, int gpio_pin);

SmokeState SmokeDetector_Read(SmokeDetector *detector);

bool SmokeDetector_Set(SmokeDetector *detector, SmokeState state);

bool SmokeDetector_HasEvent(const SmokeDetector *detector);

bool SmokeDetector_ClearEvent(SmokeDetector *detector);

#endif