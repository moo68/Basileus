#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/gl.h>
#include <cglm/cglm.h>


typedef struct {
    vec3 ambient, diffuse, specular;
    float shininess;
} PhongMaterial;

typedef struct {
    vec3 ambient, diffuse, specular;
    float shininess;

    unsigned int texture1;
} TexturedPhongMaterial;


PhongMaterial *create_phong_material(vec3 ambient, vec3 diffuse, vec3 specular,
                                     float shininess);

TexturedPhongMaterial *create_textured_phong_material(vec3 ambient, vec3 diffuse,
                                             vec3 specular, float shininess,
                                             unsigned int texture1);

#endif

