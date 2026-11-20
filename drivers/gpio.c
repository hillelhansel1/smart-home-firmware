#include "gpio.h"
#define GPIO_COUNT 32

static GPIO_InterruptHandler interrupt_handlers[GPIO_COUNT];
static void *interrupt_contexts[GPIO_COUNT];

typedef struct{
    uint32_t direction;
    uint32_t input;
    uint32_t output;

    uint32_t interrupt_enable;
    uint32_t interrupt_status;
} GPIO_Registers;

static GPIO_Registers gpio_registers;

void GPIO_Init(void){
    gpio_registers.direction = 0;
    gpio_registers.output = 0;
    gpio_registers.input = 0;
    gpio_registers.interrupt_enable = 0;
    gpio_registers.interrupt_status = 0;

    for(int i = 0; i < GPIO_COUNT; i++){
        interrupt_handlers[i] = NULL;
        interrupt_contexts[i] = NULL;
    }
}

bool GPIO_IsValidPin(int pin){
    if(pin < 0 || pin >= GPIO_COUNT){
        return false;
    }
    return true;
}

bool GPIO_Config(int pin, GPIO_DirectionState state){
    if(!GPIO_IsValidPin(pin) || (state != GPIO_INPUT && state != GPIO_OUTPUT)){
        return false;
    }

    uint32_t mask = 1u << pin;
    if(state == GPIO_OUTPUT){
        gpio_registers.direction |= mask;
    }
    else{
        gpio_registers.direction &= ~mask;
    }
    return true;
}

bool GPIO_Write(int pin, GPIO_State state){
    if(!GPIO_IsValidPin(pin) || (state != GPIO_HIGH && state != GPIO_LOW)){
        return false;
    }

    uint32_t mask = 1u << pin;
    if(gpio_registers.direction & mask){
        if(state == GPIO_HIGH){
            gpio_registers.output |= mask;
        }
        else{
            gpio_registers.output &= ~mask;
        }

        return true;
    }
    return false;
}

GPIO_State GPIO_Read(int pin){ 
    if(!GPIO_IsValidPin(pin)){
        return GPIO_LOW;
    }
    uint32_t mask = 1u << pin;
    if(gpio_registers.direction & mask){
        return (gpio_registers.output & mask) >> pin;
    }
    return (gpio_registers.input & mask) >> pin;
}

bool GPIO_SimulateInput(int pin, GPIO_State state){
        if(!GPIO_IsValidPin(pin) || (state != GPIO_HIGH && state != GPIO_LOW)){
        return false;
    }

    uint32_t mask = 1u << pin;

    if(gpio_registers.direction & mask){
        return false;
    }

    GPIO_State old_state;
    if(gpio_registers.input & mask){
        old_state = GPIO_HIGH;
    }
    else{
        old_state = GPIO_LOW;
    }

    if(state == GPIO_HIGH){
        gpio_registers.input |= mask;
    }
    else{
        gpio_registers.input &= ~mask;
    }

    if(old_state != state && (gpio_registers.interrupt_enable & mask)){
        gpio_registers.interrupt_status |= mask;

        if(interrupt_handlers[pin] != NULL){
            interrupt_handlers[pin](interrupt_contexts[pin]);
        }
    }
    return true;
}

bool GPIO_EnableInterrupt(int pin){
    if(!GPIO_IsValidPin(pin)){
        return false;
    }

    uint32_t mask = 1u << pin;

    if(gpio_registers.direction & mask){
        return false;
    }

    gpio_registers.interrupt_enable |= mask;
    return true;
}

bool GPIO_IsInterruptPending(int pin){
    if(!GPIO_IsValidPin(pin)){
        return false;
    }

    uint32_t mask = 1u << pin;

    return (gpio_registers.interrupt_status & mask) != 0;
}

bool GPIO_ClearInterrupt(int pin){
    
    if(!GPIO_IsValidPin(pin)){
        return false;
    }
    
    uint32_t mask = 1u << pin;

    gpio_registers.interrupt_status &= ~mask;
    return true;
}

bool GPIO_RegisterInterruptHandler(int pin, GPIO_InterruptHandler handler, void *context){
    if(!GPIO_IsValidPin(pin) || handler == NULL){
        return false;
    }

    interrupt_handlers[pin] = handler;
    interrupt_contexts[pin] = context;
    return true;
}
