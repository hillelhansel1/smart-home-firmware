#include "motion_detector.h"
#include "../drivers/gpio.h"

#include <stdlib.h>

static void MotionDetector_ISR(void *context);

bool MotionDetector_Init(MotionDetector *detector, int gpio_pin){
    if(detector == NULL || !GPIO_IsValidPin(gpio_pin)){
        return false;
    }

    if(!GPIO_Config(gpio_pin, GPIO_INPUT)){
        return false;
    }

    detector->gpio_pin = gpio_pin;
    detector->event_pending = false;

    if(!GPIO_RegisterInterruptHandler(gpio_pin, MotionDetector_ISR, detector)){
        return false;
    }

    if(!GPIO_EnableInterrupt(gpio_pin)){
        return false;
    }

    return true;
}

MotionState MotionDetector_Read(MotionDetector *detector){
    if(detector == NULL){
        return MOTION_NOT_DETECTED;
    }
    GPIO_State state = GPIO_Read(detector->gpio_pin);
    if(state == GPIO_HIGH){
        return MOTION_DETECTED;
    }
    return MOTION_NOT_DETECTED;
}

bool MotionDetector_Set(MotionDetector *detector, MotionState state){
    if(detector == NULL || (state != MOTION_DETECTED && state != MOTION_NOT_DETECTED)){
        return false;
    }
    
    if(state == MOTION_DETECTED){
        return GPIO_SimulateInput(detector->gpio_pin, GPIO_HIGH);
    }
        
    return GPIO_SimulateInput(detector->gpio_pin, GPIO_LOW);
}

bool MotionDetector_HasEvent(const MotionDetector *detector){
    if(detector == NULL){
        return false;
    }

    return detector->event_pending;
}

bool MotionDetector_ClearEvent(MotionDetector *detector){
    if(detector == NULL){
        return false;
    }

    detector->event_pending = false;
    return true;
}

static void MotionDetector_ISR(void *context){
    MotionDetector *detector = (MotionDetector *)context;

    if(detector == NULL){
        return;
    }

    detector->event_pending = true;

    GPIO_ClearInterrupt(detector->gpio_pin);
}