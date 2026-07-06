#ifndef RENDERER_H
#define RENDERER_H

#include <cglm/cglm.h>

#include "basileus/camera.h"
#include "basileus/mesh.h"
#include "basileus/transform.h"
#include "basileus/light.h"


typedef struct Shader Shader;

typedef struct RenderContext {
    DirectionalLight directional_light;
    PointLight point_lights[32];

    Camera camera;
    mat4 projection;

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

