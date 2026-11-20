#ifndef LIGHT_H
#define LIGHT_H
#include "../drivers/gpio.h"

typedef struct{
    int gpio_pin;
} Light;

bool Light_Init(Light *light, int gpio_pin);

void Light_TurnOn(Light *light);

void Light_TurnOff(Light *light);

void Light_Toggle(Light *light);

GPIO_State Light_GetState(Light *light);

#endif