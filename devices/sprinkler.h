#ifndef SPRINKLER_H
#define SPRINKLER_H
#include "../drivers/gpio.h"


typedef struct{
    int gpio_pin;
} Sprinkler;

bool Sprinkler_Init(Sprinkler *sprinkler, int gpio_pin);

void Sprinkler_TurnOn(Sprinkler *sprinkler);

void Sprinkler_TurnOff(Sprinkler *sprinkler);

GPIO_State Sprinkler_GetState(Sprinkler *sprinkler);

#endif