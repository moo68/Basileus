#ifndef ENTITY_H
#define ENTITY_H

#include "basileus/transform.h"

/* 
 * This number should never be UINT32_MAX or any other maximum. Maximum is
 * reserved as a sentinel value.
 */
#define MAX_ENTITIES (16)


/*
 * The system of arrays used to keep track of entity data. This only deals with
 * entities and doesn't track components in any capacity. 
 */
typedef struct {
    /*
     * This list contains the generation number for every possible entity ID. An
     * index of this array is itself an entity's ID. So for example, the value
     * contained at index 4 is the generation number of Entity 4.
     */
    uint32_t generations[MAX_ENTITIES];

    /*
     * This list is a stack that contains all possible entity IDs. The
     * used_count variable is the stack pointer. Whatever entity ID exists in
     * free_stack at the index of used_count is the next entity ID that will
     * be used upon entity creation. Creating an entity then pushes the
     * pointer forward by 1, and destroying an entity pushes the pointer
     * backward by 1.
     */
    uint32_t free_stack[MAX_ENTITIES];

    /*
     * This is the number of currently used entities. Here, "used entity"
     * means an entity that will likely have components attached to it, so it's
     * ID shouldn't ever get re-used while it's still active. This is also what
     * free_stack uses as its stack pointer.
     */
    uint32_t used_count;       
} entity_tracker_t;

/*
 * The system of arrays that tracks all possible components used by all possible
 * entities.
 */
typedef struct {
    /*
     * The sparse entity arrays include every entity via their indices (index 7
     * directly corresponds to Entity 7, etc). The actual contents of each
     * index is itself an index into the corresponding component array that
     * contains the actual component data. UINT32_MAX is a sentinel value that
     * means no entity contains the component that the array points to.
     */
    uint32_t sparse_transform_entities[MAX_ENTITIES];

    /*
     * The component arrays are dense arrays that contain actual component
     * data. Component data can be either primitive types or handles to other,
     * larger chunks of data. Components should always only ever store POD
     * (plain old data)!
     */
    Transform transform_components[MAX_ENTITIES];
    uint32_t transform_component_count;

    /*
     * The dense entity arrays directly mirror the component arrays of the
     * same component type. For example, if a component in a component array
     * at index 3 corresponds to Entity 12, then the value at index 3 in the
     * dense entity array will also correspond to Entity 12. The values stored
     * in the dense entity arrays are entity IDs. This allows easy traversal
     * from one specific component to the entity that uses that component.
     */
    uint32_t dense_transform_entities[MAX_ENTITIES];
} component_tracker_t;


entity_tracker_t create_entity_tracker(void);

uint32_t create_entity(entity_tracker_t *tracker);

void destroy_entity(entity_tracker_t *tracker, uint32_t id);

component_tracker_t create_component_tracker(void);

void create_transform_component(component_tracker_t *tracker, uint32_t id,
                                Transform transform);

void destroy_transform_component(component_tracker_t *tracker, uint32_t id);

#endif

