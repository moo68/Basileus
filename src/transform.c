#include "basileus/transform.h"

#include <cglm/cglm.h>


Transform create_transform() {
    Transform t; 

    glm_vec3_zero(t.position);
    glm_vec3_one(t.scale);

    t.rotation_axis[0] = 0.0f;
    t.rotation_axis[1] = 1.0f;
    t.rotation_axis[2] = 0.0f;
    t.rotation_radians = 0.0f;

    recalculate_transform(&t);

    return t;
}

void recalculate_transform(Transform *t) {
    glm_mat4_identity(t->model);
    glm_translate(t->model, t->position); 
    glm_rotate(t->model, t->rotation_radians, t->rotation_axis);
    glm_scale(t->model, t->scale);
}

void translate_transform(Transform *t, vec3 position) {
    t->position[0] = position[0];
    t->position[1] = position[1];
    t->position[2] = position[2];
    recalculate_transform(t);
}

void rotate_transform(Transform *t, vec3 rotation_axis, float rotation_radians) {
    t->rotation_axis[0] = rotation_axis[0];
    t->rotation_axis[1] = rotation_axis[1];
    t->rotation_axis[2] = rotation_axis[2];
    t->rotation_radians += rotation_radians;
    recalculate_transform(t);
}

void scale_transform(Transform *t, vec3 scale) {
    t->scale[0] = scale[0];
    t->scale[1] = scale[1];
    t->scale[2] = scale[2];
    recalculate_transform(t);
}

