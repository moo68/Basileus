#ifndef LIGHT_H
#define LIGHT_H

#include <cglm/cglm.h>


// TODO: Swap order of specular and diffuse
typedef struct {
    vec3 direction;

    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
} DirectionalLight;

typedef struct {
    vec3 position;

    vec3 ambient;
    vec3 specular;
    vec3 diffuse;

    float constant;
    float linear;
    float quadratic;
} PointLight;


DirectionalLight create_directional_light(vec3 direction, vec3 ambient,
                                         vec3 specular, vec3 diffuse);

PointLight create_pont_light(vec3 position, vec3 ambient, vec3 specular,
                             vec3 diffuse);

#endif

