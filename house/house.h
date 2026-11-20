#ifndef HOUSE_H
#define HOUSE_H

#include "room.h"

#define MAX_ROOMS 5

typedef struct{
    Room rooms[MAX_ROOMS];
    int number_of_rooms;
} House;

void House_Init(House *house);

void House_Update(House *house);

Room *House_AddRoom(House *house);

int House_GetRoomCount(House *house);

Room* House_GetRoom(House *house, int index);

#endif