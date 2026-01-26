#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cglm/cglm.h>


typedef struct {
    vec3 position;
    vec3 scale;
    vec3 rotation_axis;
    float rotation_radians;

    mat4 model;
} Transform;


Transform create_transform();

void recalculate_transform(Transform *t);

void translate_transform(Transform *t, vec3 position);

void rotate_transform(Transform *t, vec3 rotation_axis, float rotation_radians);

void scale_transform(Transform *t, vec3 scale);

#endif

