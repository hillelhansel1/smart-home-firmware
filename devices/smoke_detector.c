#include "smoke_detector.h"
#include "../drivers/gpio.h"

#include <stdlib.h>

static void SmokeDetector_ISR(void *context);

bool SmokeDetector_Init(SmokeDetector *detector, int gpio_pin){
    if(detector == NULL || !GPIO_IsValidPin(gpio_pin)){
        return false;
    }

    if(!GPIO_Config(gpio_pin, GPIO_INPUT)){
        return false;
    }
    detector->gpio_pin = gpio_pin;
    detector->event_pending = false;
    
    if(!GPIO_RegisterInterruptHandler(gpio_pin, SmokeDetector_ISR, detector)){
        return false;
    }

    if(!GPIO_EnableInterrupt(gpio_pin)){
        return false;
    }

    return true;
}

SmokeState SmokeDetector_Read(SmokeDetector *detector){
    if(detector == NULL){
        return SMOKE_NOT_DETECTED;
    }
    GPIO_State state = GPIO_Read(detector->gpio_pin);
    if(state == GPIO_HIGH){
        return SMOKE_DETECTED;
    }
    return SMOKE_NOT_DETECTED;
}

bool SmokeDetector_Set(SmokeDetector *detector, SmokeState state){
    if(detector == NULL || (state != SMOKE_DETECTED && state != SMOKE_NOT_DETECTED)){
        return false;
    }
    
    if(state == SMOKE_DETECTED){
        return GPIO_SimulateInput(detector->gpio_pin, GPIO_HIGH);
    }
        
    return GPIO_SimulateInput(detector->gpio_pin, GPIO_LOW);
}

bool SmokeDetector_HasEvent(const SmokeDetector *detector){
    if(detector == NULL){
        return false;
    }

    return detector->event_pending;
}

bool SmokeDetector_ClearEvent(SmokeDetector *detector){
    if(detector == NULL){
        return false;
    }

    detector->event_pending = false;
    return true;
}

static void SmokeDetector_ISR(void *context){
    SmokeDetector *detector = (SmokeDetector *)context;

    if(detector == NULL){
        return;
    }

    detector->event_pending = true;

    GPIO_ClearInterrupt(detector->gpio_pin);
}