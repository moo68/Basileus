#ifndef ENTITY_H
#define ENTITY_H

#include "basileus/transform.h"

#define MAX_ENTITIES (16)


/*
 * The system of arrays used to keep track of entity data. This only deals with
 * entities and does nothing related to any kind of component. 
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


entity_tracker_t create_entity_tracker(void);

uint32_t create_entity(entity_tracker_t *tracker);

void destroy_entity(entity_tracker_t *tracker, uint32_t id);

#endif

