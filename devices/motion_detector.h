#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <stdbool.h>

typedef enum{
    MOTION_DETECTED,
    MOTION_NOT_DETECTED
} MotionState;

typedef struct{
    int gpio_pin;
    volatile bool event_pending;
} MotionDetector;

bool MotionDetector_Init(MotionDetector *detector, int gpio_pin);

MotionState MotionDetector_Read(MotionDetector *detector);

bool MotionDetector_Set(MotionDetector *detector, MotionState state);

bool MotionDetector_HasEvent(const MotionDetector *detector);

bool MotionDetector_ClearEvent(MotionDetector *detector);

#endif