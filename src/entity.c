#include "basileus/entity.h"

#include <stdlib.h>
#include <stdbool.h>

#include "basileus/transform.h"


/*typedef struct {
    uint32_t id;
    uint32_t generation;

    bool is_active;

    Transform transform;
} Entity;*/


// TODO: Figure out the entity ID stuff, currently very broken.
// Do IDs actually correspond to entity array index? Or not? Decide.
// Hint: they probably don't!

Scene initialize_scene(void) {
    Scene s = {0};

    s.entity_count = 0;
    s.entity_capacity = 256;

    s.entity_list = calloc(s.entity_capacity, sizeof(Entity));

    return s;
}

void create_entity(Scene *scene) {
    Entity *e = NULL;

    for (uint32_t i = 0; i < scene->entity_capacity; i++) {
        e = &scene->entity_list[i];

        if (!e->is_active) {
            e->id = i;
            e->is_active = true;
            break;
        }
    }

    e->transform = create_transform();

    scene->entity_count++;
}

void destroy_entity(Scene *scene, uint32_t id) {
    Entity *e = &scene->entity_list[id];

    e->is_active = false;
    e->generation++;

    if (id != (scene->entity_count - 1)) {
        *e = scene->entity_list[scene->entity_count - 1];
        destroy_entity(scene, (scene->entity_count - 1));
    }
}

