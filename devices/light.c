#include "light.h"

#include <stdlib.h>

bool Light_Init(Light *light, int gpio_pin){
    if(light == NULL || !GPIO_IsValidPin(gpio_pin)){
        return false;
    }

    if(!GPIO_Config(gpio_pin, GPIO_OUTPUT)){
        return false;
    }

    if(!GPIO_Write(gpio_pin, GPIO_LOW)){
        return false;
    }

    light->gpio_pin = gpio_pin;
    return true;
}

void Light_TurnOn(Light *light){
    if(light == NULL){
        return;
    }
    GPIO_Write(light->gpio_pin, GPIO_HIGH);
}

void Light_TurnOff(Light *light){
    if(light == NULL){
        return;
    }
    GPIO_Write(light->gpio_pin, GPIO_LOW);
}

GPIO_State Light_GetState(Light *light){
    if(light == NULL){
        return GPIO_LOW;
    }
    return GPIO_Read(light->gpio_pin);
}

void Light_Toggle(Light *light){
    if(light == NULL){
        return;
    }
    GPIO_State current = Light_GetState(light);
    if(current == GPIO_LOW){
        Light_TurnOn(light);
    }
    else{
        Light_TurnOff(light);
    }
}