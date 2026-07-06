#include "basileus/light.h"


Light create_light(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction) {
    Light light = {0};

    glm_vec3_copy(position, light.position);
    glm_vec3_copy(ambient, light.ambient);
    glm_vec3_copy(diffuse, light.diffuse);
    glm_vec3_copy(specular, light.specular);
    glm_vec3_copy(direction, light.direction);

    return light;
}

DirectionalLight create_directional_light(vec3 direction, vec3 ambient,
                                         vec3 specular, vec3 diffuse) {
    DirectionalLight dl = {0};

    glm_vec3_copy(direction, dl.direction);
    glm_vec3_copy(ambient, dl.ambient);
    glm_vec3_copy(diffuse, dl.diffuse);
    glm_vec3_copy(specular, dl.specular);

    return dl;
}

PointLight create_pont_light(vec3 position, vec3 ambient, vec3 specular,
                             vec3 diffuse) {
    PointLight pl = {0};

    glm_vec3_copy(position, pl.position);
    glm_vec3_copy(ambient, pl.ambient);
    glm_vec3_copy(diffuse, pl.diffuse);
    glm_vec3_copy(specular, pl.specular);

    return pl;
}

