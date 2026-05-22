#include "basileus/material.h"

#include <glad/gl.h>
#include <cglm/cglm.h>

#include <stdlib.h>


PhongMaterial *create_phong_material(vec3 ambient, vec3 diffuse, vec3 specular,
                                     float shininess) {
    PhongMaterial *m = malloc(sizeof(PhongMaterial));

    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, m->ambient_light);
    glm_vec3_copy((vec3){0.5f, 0.5f, 0.5f}, m->diffuse_light);
    glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, m->specular_light);

    glm_vec3_copy(ambient, m->ambient); 
    glm_vec3_copy(diffuse, m->diffuse); 
    glm_vec3_copy(specular, m->specular); 
    m->shininess = shininess;

    return m;
}

