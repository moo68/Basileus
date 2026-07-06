#ifndef LIGHT_H
#define LIGHT_H

#include <cglm/cglm.h>


typedef struct {
    vec3 position, direction;
    vec3 ambient, specular, diffuse;
} Light;

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
} PointLight;


Light create_light(vec3 position, vec3 ambient, vec3 specular, vec3 diffuse, vec3 direction);

DirectionalLight create_directional_light(vec3 direction, vec3 ambient,
                                         vec3 specular, vec3 diffuse);

PointLight create_pont_light(vec3 position, vec3 ambient, vec3 specular,
                             vec3 diffuse);

#endif

