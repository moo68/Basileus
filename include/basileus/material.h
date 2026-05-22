#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/gl.h>
#include <cglm/cglm.h>


typedef struct {
    vec3 ambient, diffuse, specular;
    float shininess;
} PhongMaterial;


PhongMaterial *create_phong_material(vec3 ambient, vec3 diffuse, vec3 specular,
                                     float shininess);

#endif

