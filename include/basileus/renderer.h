#ifndef RENDERER_H
#define RENDERER_H

#include <cglm/cglm.h>

#include "basileus/camera.h"
#include "basileus/mesh.h"
#include "basileus/transform.h"


typedef struct Shader Shader;

typedef struct RenderContext {
    Camera camera;
    mat4 projection;

    vec3 light_position;

    float delta_time;
    float last_frame; 
} RenderContext;

typedef struct RenderObject {
    Transform transform;
    Mesh *mesh;
    Shader *shader;
    void *material;
} RenderObject;

#endif

