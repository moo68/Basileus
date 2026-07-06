#include "basileus/material.h"

#include <glad/gl.h>
#include <cglm/cglm.h>

#include <stdlib.h>


PhongMaterial *create_phong_material(vec3 ambient, vec3 diffuse, vec3 specular,
                                     float shininess) {
    PhongMaterial *m = malloc(sizeof(PhongMaterial));

    glm_vec3_copy(ambient, m->ambient); 
    glm_vec3_copy(diffuse, m->diffuse); 
    glm_vec3_copy(specular, m->specular); 
    m->shininess = shininess;

    return m;
}

TexturedPhongMaterial *create_textured_phong_material(unsigned int diffuse_tex,
                                                      unsigned int specular_tex,
                                                      float shininess) {
    TexturedPhongMaterial *m = malloc(sizeof(TexturedPhongMaterial));

    m->diffuse_tex = diffuse_tex;
    m->specular_tex = specular_tex;
    m->shininess = shininess;

    return m;
}

