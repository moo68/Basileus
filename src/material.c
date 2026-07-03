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
                                                      vec3 specular,
                                                      float shininess) {
    TexturedPhongMaterial *m = malloc(sizeof(TexturedPhongMaterial));

    //glm_vec3_copy(ambient, m->ambient); 
    //glm_vec3_copy(diffuse, m->diffuse); 
    m->diffuse_tex = diffuse_tex;
    glm_vec3_copy(specular, m->specular); 
    m->shininess = shininess;

    return m;
}

