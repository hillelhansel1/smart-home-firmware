#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "../devices/light.h"
#include "../devices/motion_detector.h"

#include <stdint.h>

#define MAX_LINKS_PER_CONTROLLER 16
#define LIGHT_MOTION_TIMEOUT_MS 4000
#define AC_MOTION_TIMEOUT_MS 2000

typedef enum{
    MOTION_CONTROL_ON,
    MOTION_CONTROL_OFF,
    MOTION_WAITING_OFF
} MotionControlState;

typedef struct{
    MotionDetector *motion_detector;
    MotionControlState state;

    Light *light_links[MAX_LINKS_PER_CONTROLLER];
    int light_link_count;

    uint64_t waiting_start_time;
} MotionController;

void MotionController_Init(MotionController *controller, MotionDetector *motion_detector);

void MotionController_AddLightLink(MotionController *controller, Light *light);

void MotionController_Update(MotionController *controller, uint64_t current_time);

bool MotionController_IsOccupied(const MotionController *controller);

#endif