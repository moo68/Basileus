#include "basileus/entity.h"

#include <stdlib.h>


entity_tracker_t create_entity_tracker(void) {
    entity_tracker_t tracker = {0};

    for (uint32_t i = 0; i < MAX_ENTITIES; i++) {
        tracker.free_stack[i] = i;
    }

    return tracker;
}

uint32_t create_entity(entity_tracker_t *tracker) {
    uint32_t entity_id = tracker->free_stack[tracker->used_count];
    tracker->used_count++;

    return entity_id;
}

void destroy_entity(entity_tracker_t *tracker, uint32_t id) {
    tracker->generations[id]++;
    tracker->used_count--;
    tracker->free_stack[tracker->used_count] = id;
}

