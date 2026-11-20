#include "ac.h"

#include <stdlib.h>

bool AC_Init(AC *ac, int gpio_pin){
    if(ac == NULL || !GPIO_IsValidPin(gpio_pin)){
        return false;
    }

    if(!GPIO_Config(gpio_pin, GPIO_OUTPUT)){
        return false;
    }

    if(!GPIO_Write(gpio_pin, GPIO_LOW)){
        return false;
    }

    ac->gpio_pin = gpio_pin;
    return true;
}

bool AC_TurnOn(AC *ac){
    if(ac == NULL){
        return false;
    }
    return GPIO_Write(ac->gpio_pin, GPIO_HIGH);
}

bool AC_TurnOff(AC *ac){
    if(ac == NULL){
        return false;
    }
    return GPIO_Write(ac->gpio_pin, GPIO_LOW);
}

GPIO_State AC_GetState(const AC *ac){
    if(ac == NULL){
        return GPIO_LOW;
    }
    return GPIO_Read(ac->gpio_pin);
}