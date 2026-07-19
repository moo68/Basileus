#ifndef ENTITY_H
#define ENTITY_H

#include "basileus/transform.h"

#include <stdbool.h>


typedef struct {
    uint32_t id;
    uint32_t generation;

    bool is_active;

    Transform transform;
} Entity;

typedef struct {
    Entity *entity_list;
    uint32_t entity_count;
    uint32_t entity_capacity;
} Scene;

#endif

