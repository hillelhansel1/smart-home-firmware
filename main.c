#include <stdio.h>
#include <windows.h>

#include "drivers/gpio.h"
#include "house/room.h"

int main(void)
{
    GPIO_Init();

    Room room;
    Room_Init(&room);

    int light_index = add_light(&room, 0);
    int motion_index = add_motion_detector(&room, 1);

    int smoke_index = add_smoke_detector(&room, 2);
    int sprinkler_index = add_sprinkler(&room, 3);

    if(light_index == -1 ||
       motion_index == -1 ||
       smoke_index == -1 ||
       sprinkler_index == -1){
        printf("Failed to initialize room devices\n");
        return 1;
    }

    link_motion_to_light(&room, motion_index, light_index);
    link_smoke_to_sprinkler(&room, sprinkler_index);

    /*
     * MOTION TEST
     */

    printf("===== MOTION TEST =====\n");

    printf("Initial light state: %d\n",
           Light_GetState(&room.light[light_index]));

    printf("\nSimulating motion...\n");

    MotionDetector_Set(
        &room.motion_detector[motion_index],
        MOTION_DETECTED
    );

    /*
     * ISR already ran here.
     * event_pending should already be true.
     *
     * But MotionController has not processed the event yet.
     */

    printf("Light BEFORE Room_Update: %d\n",
           Light_GetState(&room.light[light_index]));

    Room_Update(&room);

    printf("Light AFTER Room_Update: %d\n",
           Light_GetState(&room.light[light_index]));

    printf("Room occupied: %d\n",
           Room_IsOccupied(&room));

    /*
     * Motion disappears.
     */

    printf("\nClearing motion...\n");

    MotionDetector_Set(
        &room.motion_detector[motion_index],
        MOTION_NOT_DETECTED
    );

    Room_Update(&room);

    /*
     * Controller should now be WAITING_OFF,
     * so light should still be ON.
     */

    printf("Light while waiting for timeout: %d\n",
           Light_GetState(&room.light[light_index]));

    printf("Room occupied while waiting: %d\n",
           Room_IsOccupied(&room));

    printf("\nWaiting for motion timeout...\n");

    Sleep(4500);

    Room_Update(&room);

    printf("Light AFTER timeout: %d\n",
           Light_GetState(&room.light[light_index]));

    printf("Room occupied AFTER timeout: %d\n",
           Room_IsOccupied(&room));


    /*
     * SMOKE TEST
     */

    printf("\n\n===== SMOKE TEST =====\n");

    printf("Initial sprinkler state: %d\n",
           Sprinkler_GetState(&room.sprinkler[sprinkler_index]));

    printf("Initial smoke alert: %d\n",
           Room_HasSmokeAlert(&room));

    printf("\nSimulating smoke...\n");

    SmokeDetector_Set(
        &room.smoke_detector,
        SMOKE_DETECTED
    );

    /*
     * ISR already ran.
     * But SmokeController has not processed the event yet.
     */

    printf("Sprinkler BEFORE Room_Update: %d\n",
           Sprinkler_GetState(&room.sprinkler[sprinkler_index]));

    Room_Update(&room);

    /*
     * Controller should now be SMOKE_CONFIRMING.
     * Sprinkler should still be OFF.
     */

    printf("Sprinkler while confirming smoke: %d\n",
           Sprinkler_GetState(&room.sprinkler[sprinkler_index]));

    printf("Smoke alert while confirming: %d\n",
           Room_HasSmokeAlert(&room));

    printf("\nWaiting for smoke confirmation timeout...\n");

    Sleep(2100);

    Room_Update(&room);

    /*
     * Smoke should now be confirmed.
     */

    printf("Sprinkler AFTER confirmation: %d\n",
           Sprinkler_GetState(&room.sprinkler[sprinkler_index]));

    printf("Smoke alert AFTER confirmation: %d\n",
           Room_HasSmokeAlert(&room));

    /*
     * Smoke disappears.
     */

    printf("\nClearing smoke...\n");

    SmokeDetector_Set(
        &room.smoke_detector,
        SMOKE_NOT_DETECTED
    );

    Room_Update(&room);

    /*
     * Controller should enter SMOKE_DELAYED_OFF.
     * Sprinkler should still be ON.
     */

    printf("Sprinkler during delayed off: %d\n",
           Sprinkler_GetState(&room.sprinkler[sprinkler_index]));

    printf("\nWaiting for smoke off timeout...\n");

    Sleep(5100);

    Room_Update(&room);

    printf("Sprinkler AFTER delayed off: %d\n",
           Sprinkler_GetState(&room.sprinkler[sprinkler_index]));

    printf("Smoke alert AFTER delayed off: %d\n",
           Room_HasSmokeAlert(&room));

    return 0;
}