#include "basileus/light.h"


Light create_light(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular) {
    Light light = {0};

    glm_vec3_copy(position, light.position);
    glm_vec3_copy(ambient, light.ambient);
    glm_vec3_copy(diffuse, light.diffuse);
    glm_vec3_copy(specular, light.specular);

    return light;
}

