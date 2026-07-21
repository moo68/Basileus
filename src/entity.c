#include "basileus/entity.h"

#include <stdint.h>
#include <string.h>

#include "basileus/transform.h"


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

// TODO: Actually deal with the entity's components
void destroy_entity(entity_tracker_t *tracker, uint32_t id) {
    tracker->generations[id]++;
    tracker->used_count--;
    tracker->free_stack[tracker->used_count] = id;
}

component_tracker_t create_component_tracker(void) {
    component_tracker_t tracker = {0};

    memset(tracker.sparse_transform_entities, UINT32_MAX,
           sizeof(tracker.sparse_transform_entities));
    memset(tracker.dense_transform_entities, UINT32_MAX,
           sizeof(tracker.dense_transform_entities));

    return tracker;
}

void create_transform_component(component_tracker_t *tracker, uint32_t id,
                                Transform transform) {
    uint32_t component_index = tracker->transform_component_count;

    tracker->sparse_transform_entities[id] = component_index;

    tracker->transform_components[component_index] = transform;
    tracker->transform_component_count++;

    tracker->dense_transform_entities[component_index] = id;
}

void destroy_transform_component(component_tracker_t *tracker, uint32_t id) {
    uint32_t component_index = tracker->sparse_transform_entities[id];
    uint32_t last_component_index = tracker->transform_component_count - 1;

    Transform *last_component = &tracker->transform_components[last_component_index];

    // Delete component data, keeping the array dense
    if (component_index != last_component_index) {
        memcpy(&tracker->transform_components[component_index], last_component,
               sizeof(Transform));
    }

    memset(last_component, 0, sizeof(Transform));
    tracker->transform_component_count--;

    // Delete entity ID reference, keeping the array dense
    if (component_index != last_component_index) {
        memcpy(&tracker->dense_transform_entities[component_index],
               &tracker->dense_transform_entities[last_component_index],
               sizeof(uint32_t));
    }

    memset(&tracker->dense_transform_entities[last_component_index], UINT32_MAX,
           sizeof(uint32_t));

    // Delete sparse entity ID
    memset(&tracker->sparse_transform_entities[id], UINT32_MAX, sizeof(uint32_t));
}

