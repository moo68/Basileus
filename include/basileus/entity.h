#ifndef ENTITY_H
#define ENTITY_H

#include "basileus/transform.h"
#include "basileus/renderer.h"
#include "basileus/light.h"

#include <stdbool.h>

/* 
 * This number should never be UINT32_MAX or any other maximum. Maximum is
 * reserved as a sentinel value.
 */
#define MAX_ENTITIES (16)


/*
 * The purpose of EntityHandle is to give functions outside the EntityTracker
 * access to both an entity's ID value as well as its generation. Thus, a
 * requested entity can be checked against the current active generation,
 * preventing nasty mishaps.
 */
typedef struct {
    uint32_t id;
    uint32_t generation;
} EntityHandle;

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
} EntityTracker;

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
    uint32_t sparse_render_entities[MAX_ENTITIES];
    uint32_t sparse_point_light_entities[MAX_ENTITIES];

    /*
     * The component arrays are dense arrays that contain actual component
     * data. Component data can be either primitive types or handles to other,
     * larger chunks of data. Components should always only ever store POD
     * (plain old data)!
     */
    Transform transform_components[MAX_ENTITIES];
    uint32_t transform_component_count;

    RenderComponent render_components[MAX_ENTITIES];
    uint32_t render_component_count;

    PointLight point_light_components[MAX_ENTITIES];
    uint32_t point_light_component_count;

    /*
     * The dense entity arrays directly mirror the component arrays of the
     * same component type. For example, if a component in a component array
     * at index 3 corresponds to Entity 12, then the value at index 3 in the
     * dense entity array will also correspond to Entity 12. The values stored
     * in the dense entity arrays are entity IDs. This allows easy traversal
     * from one specific component to the entity that uses that component.
     */
    uint32_t dense_transform_entities[MAX_ENTITIES];
    uint32_t dense_render_entities[MAX_ENTITIES];
    uint32_t dense_point_light_entities[MAX_ENTITIES];
} ComponentTracker;


EntityTracker create_entity_tracker(void);

EntityHandle create_entity(EntityTracker *tracker);

void destroy_entity(EntityTracker *et, ComponentTracker *ct,
                    EntityHandle handle);

bool is_entity_alive(EntityTracker *et, EntityHandle handle);

ComponentTracker create_component_tracker(void);

void add_transform_component(EntityTracker *et, ComponentTracker *ct,
                             EntityHandle handle, Transform transform);

void remove_transform_component(EntityTracker *et, ComponentTracker *ct,
                                EntityHandle handle);

void add_render_component(EntityTracker *et, ComponentTracker *ct,
                          EntityHandle handle, RenderComponent render);

void remove_render_component(EntityTracker *et, ComponentTracker *ct,
                             EntityHandle handle);

void add_point_light_component(EntityTracker *et, ComponentTracker *ct,
                               EntityHandle handle, PointLight point_light);

void remove_point_light_component(EntityTracker *et, ComponentTracker *ct,
                                  EntityHandle handle);

#endif

