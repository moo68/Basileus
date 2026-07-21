#ifndef RENDERER_H
#define RENDERER_H

#define MAX_POINT_LIGHTS (32)

#include <stdbool.h>

#include <cglm/cglm.h>

#include "basileus/camera.h"
#include "basileus/mesh.h"
#include "basileus/transform.h"
#include "basileus/light.h"


typedef struct Shader Shader;

typedef struct RenderContext {
    DirectionalLight directional_light;

    PointLight point_lights[MAX_POINT_LIGHTS];
    int num_point_lights;

    Camera camera;
    mat4 projection;

    float delta_time;
    float last_frame; 

    bool is_wireframe;
} RenderContext;

typedef struct RenderObject {
    Transform transform;
    Mesh *mesh;
    Shader *shader;
    void *material;
} RenderObject;

typedef struct RenderComponent {
    Mesh *mesh;
    Shader *shader;
    void *material;
} RenderComponent;

#endif

