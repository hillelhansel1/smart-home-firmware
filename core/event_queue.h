#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.h"

#include <stdbool.h>
#include <stddef.h>

#define EVENT_QUEUE_SIZE 8

typedef struct {
    Event events[EVENT_QUEUE_SIZE];

    size_t head;
    size_t tail;
    size_t num_of_events;
} EventQueue;

void EventQueue_Init(EventQueue *queue);

bool EventQueue_IsFull(const EventQueue *queue);
bool EventQueue_IsEmpty(const EventQueue *queue);

bool EventQueue_Push(EventQueue *queue, Event event);
bool EventQueue_Pop(EventQueue *queue, Event *event);

#endif