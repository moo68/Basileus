#include "basileus/light.h"


DirectionalLight create_directional_light(vec3 direction, vec3 ambient,
                                         vec3 specular, vec3 diffuse) {
    DirectionalLight dl = {0};

    glm_vec3_copy(direction, dl.direction);

    glm_vec3_copy(ambient, dl.ambient);
    glm_vec3_copy(diffuse, dl.diffuse);
    glm_vec3_copy(specular, dl.specular);

    return dl;
}

PointLight create_point_light(vec3 position, vec3 ambient, vec3 specular,
                             vec3 diffuse, float constant, float linear,
                             float quadratic) {
    PointLight pl = {0};

    glm_vec3_copy(position, pl.position);

    glm_vec3_copy(ambient, pl.ambient);
    glm_vec3_copy(diffuse, pl.diffuse);
    glm_vec3_copy(specular, pl.specular);

    pl.constant = constant;
    pl.linear = linear;
    pl.quadratic = quadratic;

    return pl;
}

