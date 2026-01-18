#include "basileus/camera.h"

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>


Camera create_camera(float x, float y, float z, float fov, float sensitivity) {
    Camera c;

    c.position[0] = x;
    c.position[1] = y;
    c.position[2] = z;

    c.front[0] = 0.0f;
    c.front[1] = 0.0f;
    c.front[2] = -1.0f;

    c.up[0] = 0.0f;
    c.up[1] = 1.0f;
    c.up[2] = 0.0f;

    c.pitch = 0.0f;
    c.yaw = -90.0f; 
    c.fov = fov;
    c.sensitivity = sensitivity;

    return c;
}

void move_camera_forward(Camera *c, float distance) {
    vec3 displacement;

    glm_vec3_scale(c->front, distance, displacement);
    glm_vec3_add(c->position, displacement, c->position);
}

void move_camera_backward(Camera *c, float distance) {
    vec3 displacement;

    glm_vec3_scale(c->front, distance, displacement);
    glm_vec3_sub(c->position, displacement, c->position);
}

void move_camera_left(Camera *c, float distance) {
    vec3 displacement;
    vec3 left;

    glm_vec3_cross(c->front, c->up, left);
    glm_vec3_norm(left);

    glm_vec3_scale(left, distance, displacement);
    glm_vec3_sub(c->position, displacement, c->position);
}

void move_camera_right(Camera *c, float distance) {
    vec3 displacement;
    vec3 right;

    glm_vec3_cross(c->front, c->up, right);
    glm_vec3_norm(right);

    glm_vec3_scale(right, distance, displacement);
    glm_vec3_add(c->position, displacement, c->position);
}

