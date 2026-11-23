#include <assert.h>
#include <stdio.h>

#include "../drivers/gpio.h"
#include "../house/house.h"
#include "test_time.h"

int main(void)
{
    GPIO_Init();
    TestTime_SetMilliseconds(0);

    House house;
    House_Init(&house);

    Room *room = House_AddRoom(&house);
    assert(room != NULL);

    int light = add_light(room, 1);
    int motion = add_motion_detector(room, 2);
    int smoke = add_smoke_detector(room, 3);
    int sprinkler = add_sprinkler(room, 4);
    int ac = add_ac(room, 5);
    int temperature = add_temperature_sensor(room);
    int climate = add_climate_controller(room, temperature);

    assert(light >= 0);
    assert(motion >= 0);
    assert(smoke >= 0);
    assert(sprinkler >= 0);
    assert(ac >= 0);
    assert(temperature >= 0);
    assert(climate >= 0);

    link_motion_to_light(room, motion, light);
    link_smoke_to_sprinkler(room, sprinkler);
    link_motion_to_climate(room, motion, climate);
    link_climate_to_ac(room, climate, ac);

    /*
     * Motion path:
     * input -> interrupt -> MotionController -> light
     */

    MotionDetector_Set(&room->motion_detector[motion], MOTION_DETECTED);
    House_Update(&house);

    assert(Room_IsOccupied(room));
    assert(Light_GetState(&room->light[light]) == GPIO_HIGH);

    TestTime_SetMilliseconds(100);

    MotionDetector_Set(&room->motion_detector[motion], MOTION_NOT_DETECTED);
    House_Update(&house);

    assert(Room_IsOccupied(room));
    assert(Light_GetState(&room->light[light]) == GPIO_HIGH);

    TestTime_SetMilliseconds(100 + LIGHT_MOTION_TIMEOUT_MS);
    House_Update(&house);

    assert(!Room_IsOccupied(room));
    assert(Light_GetState(&room->light[light]) == GPIO_LOW);

    /*
     * Smoke path:
     * input -> interrupt -> confirmation -> sprinkler -> delayed off
     */

    TestTime_SetMilliseconds(20000);

    SmokeDetector_Set(&room->smoke_detector, SMOKE_DETECTED);
    House_Update(&house);

    assert(!Room_HasSmokeAlert(room));
    assert(Sprinkler_GetState(&room->sprinkler[sprinkler]) == GPIO_LOW);

    TestTime_AdvanceMilliseconds(SMOKE_CONFIRMATION_TIMEOUT_MS);
    House_Update(&house);

    assert(Room_HasSmokeAlert(room));
    assert(Sprinkler_GetState(&room->sprinkler[sprinkler]) == GPIO_HIGH);

    SmokeDetector_Set(&room->smoke_detector, SMOKE_NOT_DETECTED);
    House_Update(&house);

    assert(Sprinkler_GetState(&room->sprinkler[sprinkler]) == GPIO_HIGH);

    TestTime_AdvanceMilliseconds(SMOKE_DELAYED_OFF_TIMEOUT_MS);
    House_Update(&house);

    assert(!Room_HasSmokeAlert(room));
    assert(Sprinkler_GetState(&room->sprinkler[sprinkler]) == GPIO_LOW);

    /*
     * Climate path:
     * occupancy + temperature -> ClimateController -> AC
     */

    ClimateController_SetTargetTemperature(
        &room->climate_controllers[climate],
        22.0f
    );

    /*
     * Make the room occupied again.
     * Climate control is linked to the MotionController.
     */

    MotionDetector_Set(&room->motion_detector[motion], MOTION_DETECTED);
    House_Update(&house);

    assert(Room_IsOccupied(room));

    /*
     * 23 C is target + margin, so AC should turn on.
     */

    TemperatureSensor_Set(&room->temperature_sensor[temperature], 23.0f);
    House_Update(&house);

    assert(AC_GetState(&room->ac[ac]) == GPIO_HIGH);

    /*
     * 22 C is inside the hysteresis band.
     * AC should preserve its previous ON state.
     */

    TemperatureSensor_Set(&room->temperature_sensor[temperature], 22.0f);
    House_Update(&house);

    assert(AC_GetState(&room->ac[ac]) == GPIO_HIGH);

    /*
     * 21 C is target - margin, so AC should turn off.
     */

    TemperatureSensor_Set(&room->temperature_sensor[temperature], 21.0f);
    House_Update(&house);

    assert(AC_GetState(&room->ac[ac]) == GPIO_LOW);

    /*
     * Verify occupancy also affects climate control.
     */

    TemperatureSensor_Set(&room->temperature_sensor[temperature], 23.0f);
    House_Update(&house);

    assert(AC_GetState(&room->ac[ac]) == GPIO_HIGH);

    MotionDetector_Set(&room->motion_detector[motion], MOTION_NOT_DETECTED);
    House_Update(&house);

    assert(AC_GetState(&room->ac[ac]) == GPIO_HIGH);

    TestTime_AdvanceMilliseconds(LIGHT_MOTION_TIMEOUT_MS);
    House_Update(&house);

    assert(!Room_IsOccupied(room));
    assert(AC_GetState(&room->ac[ac]) == GPIO_LOW);

    printf("[PASS] House/Room integration\n");
    printf("All integration tests passed.\n");

    return 0;
}