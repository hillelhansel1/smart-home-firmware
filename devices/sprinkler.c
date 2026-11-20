#include "sprinkler.h"

#include <stdlib.h>

bool Sprinkler_Init(Sprinkler *sprinkler, int gpio_pin){
    if(sprinkler == NULL || !GPIO_IsValidPin(gpio_pin)){
        return false;
    }
    
    if(!GPIO_Config(gpio_pin, GPIO_OUTPUT)){
        return false;
    }

    if(!GPIO_Write(gpio_pin, GPIO_LOW)){
        return false;
    }

    sprinkler->gpio_pin = gpio_pin;
    return true;
}

void Sprinkler_TurnOn(Sprinkler *sprinkler){
    if(sprinkler == NULL){
        return;
    }
    
    GPIO_Write(sprinkler->gpio_pin, GPIO_HIGH);
}

void Sprinkler_TurnOff(Sprinkler *sprinkler){
    if(sprinkler == NULL){
        return;
    }

    GPIO_Write(sprinkler->gpio_pin, GPIO_LOW);
}

GPIO_State Sprinkler_GetState(Sprinkler *sprinkler){
    if(sprinkler == NULL){
        return GPIO_LOW;
    }

    return GPIO_Read(sprinkler->gpio_pin);
}