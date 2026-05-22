#ifndef LIGHT_H
#define LIGHT_H

#include <cglm/cglm.h>


typedef struct {
    vec3 position;
    vec3 ambient, specular, diffuse;
} Light;


Light create_light(vec3 position, vec3 ambient, vec3 specular, vec3 diffuse);

#endif

