#ifndef AC_H
#define AC_H
#include "../drivers/gpio.h"

typedef struct
{
    int gpio_pin;
} AC;

bool AC_Init(AC *ac, int gpio_pin);

bool AC_TurnOn(AC *ac);

bool AC_TurnOff(AC *ac);

GPIO_State AC_GetState(const AC *ac);

#endif