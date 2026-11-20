#ifndef EVENT_H
#define EVENT_H

typedef enum {
    EVENT_NONE,
    EVENT_MOTION_CHANGED,
    EVENT_SMOKE_CHANGED,
    EVENT_TEMPERATURE_CHANGED,
    EVENT_TIMEOUT
} EventType;

typedef struct {
    EventType type;
    int device_index;
    float value;
} Event;

#endif