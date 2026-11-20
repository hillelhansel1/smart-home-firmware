#include <assert.h>
#include <stdio.h>

#include "../drivers/gpio.h"
#include "../devices/light.h"
#include "../devices/ac.h"
#include "../devices/sprinkler.h"
#include "../devices/motion_detector.h"
#include "../devices/smoke_detector.h"
#include "../devices/temperature_sensor.h"
#include "../controllers/motion_controller.h"
#include "../controllers/smoke_controller.h"
#include "../controllers/temperature_controller.h"

static void test_gpio(void)
{
    GPIO_Init();

    assert(!GPIO_IsValidPin(-1));
    assert(GPIO_IsValidPin(0));
    assert(GPIO_IsValidPin(31));
    assert(!GPIO_IsValidPin(32));

    assert(GPIO_Config(1, GPIO_OUTPUT));
    assert(GPIO_Write(1, GPIO_HIGH));
    assert(GPIO_Read(1) == GPIO_HIGH);
    assert(GPIO_Write(1, GPIO_LOW));
    assert(GPIO_Read(1) == GPIO_LOW);
    assert(!GPIO_SimulateInput(1, GPIO_HIGH));

    assert(GPIO_Config(2, GPIO_INPUT));
    assert(GPIO_EnableInterrupt(2));
    assert(!GPIO_IsInterruptPending(2));

    assert(GPIO_SimulateInput(2, GPIO_HIGH));
    assert(GPIO_Read(2) == GPIO_HIGH);
    assert(GPIO_IsInterruptPending(2));

    assert(GPIO_ClearInterrupt(2));
    assert(!GPIO_IsInterruptPending(2));

    /* Same state: no new edge, therefore no new interrupt. */
    assert(GPIO_SimulateInput(2, GPIO_HIGH));
    assert(!GPIO_IsInterruptPending(2));

    assert(GPIO_SimulateInput(2, GPIO_LOW));
    assert(GPIO_IsInterruptPending(2));

    printf("[PASS] GPIO\n");
}

static void test_motion_controller(void)
{
    GPIO_Init();

    Light light;
    AC ac;
    MotionDetector detector;
    MotionController controller;

    assert(Light_Init(&light, 1));
    assert(AC_Init(&ac, 2));
    assert(MotionDetector_Init(&detector, 3));

    MotionController_Init(&controller, &detector);
    MotionController_AddLightLink(&controller, &light);
    MotionController_AddACLink(&controller, &ac);

    assert(controller.state == MOTION_CONTROL_OFF);
    assert(Light_GetState(&light) == GPIO_LOW);
    assert(AC_GetState(&ac) == GPIO_LOW);

    MotionDetector_Set(&detector, MOTION_DETECTED);
    MotionController_Update(&controller, 100);

    assert(controller.state == MOTION_CONTROL_ON);
    assert(Light_GetState(&light) == GPIO_HIGH);
    assert(AC_GetState(&ac) == GPIO_HIGH);
    assert(!MotionDetector_IsInterruptPending(&detector));

    MotionDetector_Set(&detector, MOTION_NOT_DETECTED);
    MotionController_Update(&controller, 200);

    assert(controller.state == MOTION_WAITING_OFF);
    assert(Light_GetState(&light) == GPIO_HIGH);
    assert(AC_GetState(&ac) == GPIO_HIGH);

    MotionController_Update(&controller, 2199);
    assert(AC_GetState(&ac) == GPIO_HIGH);

    MotionController_Update(&controller, 2200);
    assert(controller.state == MOTION_WAITING_OFF);
    assert(AC_GetState(&ac) == GPIO_LOW);
    assert(Light_GetState(&light) == GPIO_HIGH);

    /* Motion returns before the light timeout. */
    MotionDetector_Set(&detector, MOTION_DETECTED);
    MotionController_Update(&controller, 3000);
    assert(controller.state == MOTION_CONTROL_ON);
    assert(Light_GetState(&light) == GPIO_HIGH);
    assert(AC_GetState(&ac) == GPIO_HIGH);

    MotionDetector_Set(&detector, MOTION_NOT_DETECTED);
    MotionController_Update(&controller, 4000);
    assert(controller.state == MOTION_WAITING_OFF);

    MotionController_Update(&controller, 14000);
    assert(controller.state == MOTION_CONTROL_OFF);
    assert(Light_GetState(&light) == GPIO_LOW);
    assert(AC_GetState(&ac) == GPIO_LOW);

    printf("[PASS] MotionController\n");
}

static void test_smoke_controller(void)
{
    GPIO_Init();

    SmokeDetector detector;
    Sprinkler sprinkler;
    SmokeController controller;

    assert(SmokeDetector_Init(&detector, 4));
    assert(Sprinkler_Init(&sprinkler, 5));

    SmokeController_Init(&controller, &detector);
    SmokeController_AddSprinklerLink(&controller, &sprinkler);

    /* False alarm: smoke disappears before confirmation timeout. */
    SmokeDetector_Set(&detector, SMOKE_DETECTED);
    SmokeController_Update(&controller, 100);
    assert(controller.state == SMOKE_CONFIRMING);

    SmokeDetector_Set(&detector, SMOKE_NOT_DETECTED);
    SmokeController_Update(&controller, 1000);
    assert(controller.state == SMOKE_CONTROL_OFF);
    assert(Sprinkler_GetState(&sprinkler) == GPIO_LOW);

    /* Continuous smoke is confirmed after 2 seconds. */
    SmokeDetector_Set(&detector, SMOKE_DETECTED);
    SmokeController_Update(&controller, 2000);
    assert(controller.state == SMOKE_CONFIRMING);

    SmokeController_Update(&controller, 3999);
    assert(Sprinkler_GetState(&sprinkler) == GPIO_LOW);

    SmokeController_Update(&controller, 4000);
    assert(controller.state == SMOKE_CONTROL_ON);
    assert(Sprinkler_GetState(&sprinkler) == GPIO_HIGH);

    /* Smoke clears, but sprinkler remains on during delayed-off period. */
    SmokeDetector_Set(&detector, SMOKE_NOT_DETECTED);
    SmokeController_Update(&controller, 5000);
    assert(controller.state == SMOKE_DELAYED_OFF);
    assert(Sprinkler_GetState(&sprinkler) == GPIO_HIGH);

    SmokeController_Update(&controller, 9999);
    assert(Sprinkler_GetState(&sprinkler) == GPIO_HIGH);

    SmokeController_Update(&controller, 10000);
    assert(controller.state == SMOKE_CONTROL_OFF);
    assert(Sprinkler_GetState(&sprinkler) == GPIO_LOW);

    printf("[PASS] SmokeController\n");
}

static void test_temperature_controller(void)
{
    GPIO_Init();

    TemperatureSensor sensor;
    AC ac;
    TemperatureController controller;

    TemperatureSensor_Init(&sensor);
    assert(AC_Init(&ac, 6));
    AC_SetTemperature(&ac, 22.0f);

    TemperatureController_Init(&controller, &sensor);
    TemperatureController_AddACLink(&controller, &ac);

    TemperatureSensor_Set(&sensor, 23.0f);
    TemperatureController_Update(&controller);
    assert(AC_GetState(&ac) == GPIO_HIGH);

    /* Inside hysteresis band: preserve previous state. */
    TemperatureSensor_Set(&sensor, 22.0f);
    TemperatureController_Update(&controller);
    assert(AC_GetState(&ac) == GPIO_HIGH);

    TemperatureSensor_Set(&sensor, 21.0f);
    TemperatureController_Update(&controller);
    assert(AC_GetState(&ac) == GPIO_LOW);

    /* Inside hysteresis band again: remain off. */
    TemperatureSensor_Set(&sensor, 22.0f);
    TemperatureController_Update(&controller);
    assert(AC_GetState(&ac) == GPIO_LOW);

    printf("[PASS] TemperatureController\n");
}

int main(void)
{
    test_gpio();
    test_motion_controller();
    test_smoke_controller();
    test_temperature_controller();

    printf("\nAll component tests passed.\n");
    return 0;
}
