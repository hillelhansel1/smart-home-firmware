#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*GPIO_InterruptHandler)(void *context);

typedef enum{
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} GPIO_State;

typedef enum{
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1
}GPIO_DirectionState;

void GPIO_Init(void);

bool GPIO_IsValidPin(int pin);

bool GPIO_Config(int pin, GPIO_DirectionState state);

bool GPIO_Write(int pin, GPIO_State state);

GPIO_State GPIO_Read(int pin);

bool GPIO_SimulateInput(int pin, GPIO_State state);

bool GPIO_EnableInterrupt(int pin);

bool GPIO_IsInterruptPending(int pin);

bool GPIO_ClearInterrupt(int pin);

bool GPIO_RegisterInterruptHandler(int pin, GPIO_InterruptHandler handler, void *context);

#endif