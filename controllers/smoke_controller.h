#ifndef SMOKE_CONTROLLER_H
#define SMOKE_CONTROLLER_H
#include "../devices/smoke_detector.h"
#include "../devices/sprinkler.h"
#include <stdint.h>

#define MAX_LINKS_PER_CONTROLLER 16
#define SMOKE_CONFIRMATION_TIMEOUT_MS 2000
#define SMOKE_DELAYED_OFF_TIMEOUT_MS 5000


typedef enum{
    SMOKE_CONTROL_OFF,
    SMOKE_CONFIRMING,
    SMOKE_CONTROL_ON,
    SMOKE_DELAYED_OFF
} SmokeControlState;

typedef struct{
    SmokeDetector *smoke_detector;
    SmokeControlState state;

    Sprinkler *sprinkler[MAX_LINKS_PER_CONTROLLER];
    int sprinkler_count;

    uint64_t waiting_start_time;
} SmokeController;

void SmokeController_Init(SmokeController *controller, SmokeDetector *smoke_detector);

void SmokeController_AddSprinklerLink(SmokeController *controller, Sprinkler *sprinkler);

void SmokeController_Update(SmokeController *controller, uint64_t current_time);

SmokeControlState SmokeController_GetState(const SmokeController *controller);

#endif