#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>


typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;

    float pitch;
    float yaw; 
    float fov;
    float sensitivity;
} Camera;


Camera create_camera(float x, float y, float z, float fov, float sensitivity);

void move_camera_forward(Camera *c, float distance);

void move_camera_backward(Camera *c, float distance);

void move_camera_left(Camera *c, float distance);

void move_camera_right(Camera *c, float distance);

#endif
