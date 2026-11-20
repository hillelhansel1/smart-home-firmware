#include "event_queue.h"

void EventQueue_Init(EventQueue *queue){
    if(queue == NULL){
        return;
    }

    queue->tail = 0;
    queue->head = 0;
    queue->num_of_events = 0;
}

bool EventQueue_IsFull(const EventQueue *queue){
    if(queue == NULL){
        return false;
    }

    return queue->num_of_events == EVENT_QUEUE_SIZE;
}

bool EventQueue_IsEmpty(const EventQueue *queue){
    if(queue == NULL){
        return false;
    }

    return queue->num_of_events == 0;
}

bool EventQueue_Pop(EventQueue *queue, Event *event){
    if(queue == NULL || event == NULL){
        return false;
    }

    if(EventQueue_IsEmpty(queue)){
        return false;
    }

    *event = queue->events[queue->tail];
    queue->tail = (queue->tail + 1) % EVENT_QUEUE_SIZE;
    queue->num_of_events--;
    return true;
}

bool EventQueue_Push(EventQueue *queue, Event event){
    if(queue == NULL){
        return false;
    }

    if(EventQueue_IsFull(queue)){
        return false;
    }

    queue->events[queue->head] = event;
    queue->head = (queue->head + 1) % EVENT_QUEUE_SIZE;
    queue->num_of_events++;
    return true;
}

