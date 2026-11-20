#include "house.h"
#include <stdlib.h>

void House_Init(House *house){
    if(house == NULL){
        return;
    }
    house->number_of_rooms = 0;
}

void House_Update(House *house){
    if(house == NULL){
        return;
    }
    for(int i = 0; i < House_GetRoomCount(house); i++){
        Room *room = House_GetRoom(house, i);
        Room_Update(room);
    }
}

Room *House_AddRoom(House *house){
    if(house == NULL){
        return NULL;
    }

    if(house->number_of_rooms >= MAX_ROOMS){
        return NULL;
    }

    Room* new_room = &(house->rooms[house->number_of_rooms]);
    Room_Init(new_room);
    house->number_of_rooms++;
    return new_room;
}

int House_GetRoomCount(House *house){
    if(house == NULL){
        return 0;
    }
    return house->number_of_rooms;
}

Room* House_GetRoom(House *house, int index){
    if(house == NULL || index < 0 || index >= house->number_of_rooms){
        return NULL;
    }

    return &house->rooms[index];
}