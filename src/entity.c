#include "basileus/entity.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "basileus/transform.h"


EntityTracker create_entity_tracker(void) {
    EntityTracker tracker = {0};

    // Entity IDs are based on their index in the free_stack
    for (uint32_t i = 0; i < MAX_ENTITIES; i++) {
        tracker.free_stack[i] = i;
    }

    return tracker;
}

EntityHandle create_entity(EntityTracker *tracker) {
    // Don't create a new entity if we're at the max number of entities
    if (tracker->used_count == MAX_ENTITIES) {
        EntityHandle invalid_handle = {
            .id = UINT32_MAX,
            .generation = UINT32_MAX
        };

        return invalid_handle;
    }

    // Create the new entity
    uint32_t entity_id = tracker->free_stack[tracker->used_count];
    tracker->used_count++;
    uint32_t entity_generation = tracker->generations[entity_id];

    EntityHandle handle = {
        .id = entity_id,
        .generation = entity_generation
    };

    return handle;
}

void destroy_entity(EntityTracker *et, ComponentTracker *ct, EntityHandle handle) {
    if (!is_entity_alive(et, handle)) {
        // TODO: add a return type to indicate success/failure?
        return;
    }

    // First, remove all components that exist for this entity
    if (ct->sparse_transform_entities[handle.id] != UINT32_MAX) {
        remove_transform_component(et, ct, handle);
    }

    // Then, update the entity tracker appropriately
    et->generations[handle.id]++;
    et->used_count--;
    et->free_stack[et->used_count] = handle.id;
}

bool is_entity_alive(EntityTracker *et, EntityHandle handle) {
    if (handle.id >= MAX_ENTITIES) {
        return false;
    }

    uint32_t current_generation = et->generations[handle.id];
    if (current_generation != handle.generation) {
        return false;
    }
    else {
        return true;
    }
}

ComponentTracker create_component_tracker(void) {
    ComponentTracker ct = {0};

    // UINT32_MAX is the sentinel value for "this doesn't exist"
    memset(ct.sparse_transform_entities, UINT32_MAX,
           sizeof(ct.sparse_transform_entities));
    memset(ct.dense_transform_entities, UINT32_MAX,
           sizeof(ct.dense_transform_entities));

    return ct;
}

void add_transform_component(EntityTracker *et, ComponentTracker *ct, 
                             EntityHandle handle, Transform transform) {
    if (!is_entity_alive(et, handle)) {
        // TODO: add a return type to indicate success/failure?
        return;
    }

    // Check that the entity doesn't already have a same component
    if (ct->sparse_transform_entities[handle.id] != UINT32_MAX) {
        // TODO: add a return type to indicate success/failure?
        return;
    }

    // Actually add the component and update associated data
    uint32_t component_index = ct->transform_component_count;

    ct->sparse_transform_entities[handle.id] = component_index;

    ct->transform_components[component_index] = transform;
    ct->transform_component_count++;

    ct->dense_transform_entities[component_index] = handle.id;
}

void remove_transform_component(EntityTracker *et, ComponentTracker *ct,
                                EntityHandle handle) {
    if (!is_entity_alive(et, handle)) {
        // TODO: add a return type to indicate success/failure?
        return;
    }

    // Check that the component being removed actually exists
    if (ct->sparse_transform_entities[handle.id] == UINT32_MAX) {
        // TODO: add a return type to indicate success/failure?
        return;
    }

    uint32_t component_index = ct->sparse_transform_entities[handle.id];
    uint32_t last_component_index = ct->transform_component_count - 1;

    Transform *last_component = &ct->transform_components[last_component_index];

    // Delete component data, keeping the array dense
    if (component_index != last_component_index) {
        memcpy(&ct->transform_components[component_index], last_component,
               sizeof(Transform));

        // Make sure to set the last component's sparse array data to its new location
        uint32_t last_component_id = ct->dense_transform_entities[last_component_index];
        ct->sparse_transform_entities[last_component_id] = component_index;
    }

    memset(last_component, 0, sizeof(Transform));
    ct->transform_component_count--;

    // Delete sparse entity ID
    ct->sparse_transform_entities[handle.id] = UINT32_MAX;

    // Delete entity ID reference, keeping the array dense
    if (component_index != last_component_index) {
        ct->dense_transform_entities[component_index] = 
            ct->dense_transform_entities[last_component_index];
    }

    ct->dense_transform_entities[last_component_index] = UINT32_MAX;
}

