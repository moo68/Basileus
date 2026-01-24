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

void look_camera_around(Camera *c, float x_pos, float y_pos, float *last_x, 
        float *last_y, bool *first_mouse);

void set_view_matrix(Camera *c, mat4 *view);

#endif
