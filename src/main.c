#include <stdio.h> 
#include <stdbool.h>

#include <glad/gl.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>

#include <cglm/cglm.h>
#include <stb_image.h>

#include "basileus/mesh.h"
#include "basileus/transform.h"
#include "basileus/camera.h"
#include "basileus/material.h"
#include "basileus/shader_utils.h"
#include "basileus/texture_utils.h"
#include "basileus/debug_geometry.h"
#include "basileus/renderer.h"
#include "basileus/light.h"
#include "basileus/entity.h"


SDL_Window *create_window();
SDL_GLContext create_context(SDL_Window *window);
void window_resize_callback(SDL_Event *event);
void key_callback(SDL_Event *event);
void mouse_callback(SDL_Event *event);
void process_input(void);


RenderContext render_context = {0};

int main(int argc, char *argv[]) { 
    (void)argv;
    (void)argc;

    // Initialize SDL and OpenGL, creating a window and context
    SDL_Window *window = create_window();
    if (!window) {
        return -1;
    }

    SDL_SetWindowRelativeMouseMode(window, true);

    SDL_GLContext gl_context = create_context(window);
    if (!gl_context) {
        return -1;
    }

    // Set the initial OpenGL viewport
    int window_width, window_height;
    SDL_GetWindowSizeInPixels(window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    // Turn on VSync
    SDL_GL_SetSwapInterval(0);

    // Shaders
    /*unsigned int textured_shader_program = create_shader_program("assets/shaders/textured_vert.glsl",
            "assets/shaders/textured_frag.glsl");*/
    unsigned int phong_shader_program = create_shader_program("assets/shaders/phong_vert.glsl",
            "assets/shaders/phong_frag.glsl");
    unsigned int light_source_shader_program = create_shader_program("assets/shaders/light_source_vert.glsl",
            "assets/shaders/light_source_frag.glsl");
    /*unsigned int textured_phong_shader_program = create_shader_program("assets/shaders/textured_phong_vert.glsl",
            "assets/shaders/textured_phong_frag.glsl");*/
 
    PhongShader *phong_shader = create_phong_shader(phong_shader_program);
    //SimpleShader *light_source_shader = create_simple_shader(light_source_shader_program);
    //TexturedPhongShader *textured_phong_shader = create_textured_phong_shader(textured_phong_shader_program);

    // Meshes and Buffers
    int duck_mesh_count = 0;
    Mesh *duck_meshes = load_gltf_file("assets/models/duck/Duck.gltf", &duck_mesh_count);
    int cube_mesh_count = 0;
    Mesh *cube_meshes = load_gltf_file("assets/models/cube/Cube.gltf", &cube_mesh_count);

    // Textures 
    unsigned int texture = load_texture("assets/textures/container2.png"); 
    unsigned int specular_map = load_texture("assets/textures/container2_specular.png"); 

    // Render context 
    Camera camera = create_camera(0.0f, 0.0f, 3.0f, 45.0f, 0.1f);
    render_context.camera = camera;

    glm_mat4_identity(render_context.projection);
    glm_perspective(glm_rad(45.0f), (float)window_width / (float)window_height, 
                0.1f, 100.0f, render_context.projection);

    render_context.is_wireframe = false;
 
    // TODO: Lights should store a Transform for their position, not their own separate vec3?
    vec3 light_pos, light_amb, light_diff, light_spec, light_dir;
    glm_vec3_copy((vec3){3.0f, 1.0f, -3.0f}, light_pos);
    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, light_amb);
    glm_vec3_copy((vec3){0.8f, 0.8f, 0.8f}, light_diff);
    glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, light_spec);
    glm_vec3_copy((vec3){-0.2f, -1.0f, -0.3f}, light_dir);
    render_context.directional_light = 
        create_directional_light(light_dir, light_amb, light_diff, light_spec);

    // Materials
    vec3 ambient, diffuse, specular;
    glm_vec3_copy((vec3){1.0f, 0.5f, 0.31f}, ambient); 
    glm_vec3_copy((vec3){1.0f, 0.5f, 0.31f}, diffuse); 
    glm_vec3_copy((vec3){0.5f, 0.5f, 0.5f}, specular); 
    float shininess = 32.0f;
    PhongMaterial *phong_mat = create_phong_material(ambient, diffuse, specular, shininess);

    /*TexturedPhongMaterial *textured_phong_mat =
        create_textured_phong_material(texture, specular_map, shininess);*/

    // RenderComponents
    RenderComponent duck_render = {
        .mesh = &duck_meshes[0],
        .shader = (Shader *)(phong_shader), 
        .material = phong_mat
    };
 
    // Transforms
    Transform object_transform = create_transform();
    translate_transform(&object_transform, (vec3){3.0f, 0.0f, -3.0f});

    Transform light_transform = create_transform();
    translate_transform(&light_transform, (vec3){0.0f, 0.0f, 0.0f});
    scale_transform(&light_transform, (vec3){0.25f, 0.25f, 0.25f});

    Transform duck_transform = create_transform();
    scale_transform(&duck_transform, (vec3){0.01f, 0.01f, 0.01f});

    // Entity Component System
    EntityTracker entity_tracker = create_entity_tracker();
    ComponentTracker component_tracker = create_component_tracker();

    EntityHandle entity0 = create_entity(&entity_tracker);
    add_transform_component(&entity_tracker, &component_tracker, entity0, duck_transform);
    add_render_component(&entity_tracker, &component_tracker, entity0, duck_render);

    // Render Loop
    bool is_running = true;
    SDL_Event event = {0};
    render_context.last_frame = SDL_GetTicks();

    uint64_t last_time = SDL_GetTicks();
    int frame_count = 0;
    double fps = 0.0;

    while (is_running) {
        uint64_t current_frame = SDL_GetTicks();
        render_context.delta_time = (current_frame - render_context.last_frame) / 1000.0f;
        render_context.last_frame = current_frame;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    is_running = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    window_resize_callback(&event);
                    break;
                case SDL_EVENT_KEY_DOWN:
                    key_callback(&event);
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    mouse_callback(&event);
                    break;
            }
        }

        process_input();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        set_view_matrix(&(render_context.camera));

        // Draw all renderable entities
        for (uint32_t i = 0; i < component_tracker.render_component_count; i++) {
            // First, fetch entity data
            RenderComponent curr_render_component = 
                component_tracker.render_components[i];

            uint32_t curr_entity_id = component_tracker.dense_render_entities[i];
            uint32_t curr_transform_index = 
                component_tracker.sparse_transform_entities[curr_entity_id];

            Transform curr_transform = 
                component_tracker.transform_components[curr_transform_index];

            // Then, upload that data to the GPU and draw on screen
            glBindVertexArray(curr_render_component.mesh->vao);

            glUseProgram(curr_render_component.shader->shader_program);

            curr_render_component.shader->
                upload_uniforms(&render_context, &curr_render_component, &curr_transform);

            glDrawElements(GL_TRIANGLES, curr_render_component.mesh->index_count,
                           GL_UNSIGNED_INT, 0);
        }

        frame_count++;
        uint64_t current_time = SDL_GetTicks();
        if (current_time - last_time >= 1000) { // every 1 second
            fps = frame_count / ((current_time - last_time) / 1000.0);
            SDL_Log("FPS: %.2f", fps);
            frame_count = 0;
            last_time = current_time;
        }

        // "sunrise" and "sunset"
        float angle = (SDL_GetTicks() / 1000.0f) * 0.5;
        render_context.directional_light.direction[0] = cos(angle);
        render_context.directional_light.direction[1] = sin(angle);

        GLenum error;
        while ((error = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL error: 0x%x\n", error);
        }

        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    // Shutdown cleanup
    cleanup_mesh(&duck_meshes[0]);
    cleanup_mesh(&cube_meshes[0]);
    glDeleteProgram(light_source_shader_program);
    glDeleteProgram(phong_shader_program);
    //glDeleteProgram(textured_phong_shader_program);

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


// Function definitions
SDL_Window *create_window() {
    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to initialize SDL3!");
        return NULL;
    }
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initialized SDL3");

    // Set OpenGL version and profile
#ifndef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    int gl_major_version = -1;
    int gl_minor_version = -1;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_version);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Using OpenGL version %d.%d",
                gl_major_version, gl_minor_version);

    // Set other OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Create the application's window
    SDL_Window *window = SDL_CreateWindow("Caverns Test", 1200, 800,
                                          SDL_WINDOW_OPENGL |
                                          SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to create window!");
        SDL_Quit();
        return NULL;
    }

    return window;
}

SDL_GLContext create_context(SDL_Window *window) {
    // Create the OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
                        "Failed to create OpenGL context!");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    // Load GLAD so OpenGL functions work properly
    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to initialize GLAD!");
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initialized GLAD");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    return gl_context;
}

void window_resize_callback(SDL_Event *event) {
    glViewport(0, 0, event->window.data1, event->window.data2);
    float aspect_ratio = (float)event->window.data1 / (float)event->window.data2;
    glm_perspective(glm_rad(45.0f), aspect_ratio, 0.1f, 100.0f,
                    render_context.projection);
}

void key_callback(SDL_Event *event) {
    // SDL fires KEY_DOWN repeatedly if key-repeat is on
    if (event->key.repeat) return;

    // TODO: actually implement this
    /*if (event->key.key == SDLK_ESCAPE) {
        is_running = false;
    }*/
    else if (!render_context.is_wireframe && event->key.key == SDLK_TAB) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        render_context.is_wireframe = true;
    }
    else if (render_context.is_wireframe && event->key.key == SDLK_TAB) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        render_context.is_wireframe = false;
    }
}

void mouse_callback(SDL_Event *event) {
    look_camera_around(&(render_context.camera), event->motion.xrel,
                       event->motion.yrel);
}

void process_input(void) {
    const bool *keystate = SDL_GetKeyboardState(NULL);

    float speed = 2.5f * render_context.delta_time;
    if (keystate[SDL_SCANCODE_LSHIFT]) {
        speed = 12.5f * render_context.delta_time;
    }
    if (keystate[SDL_SCANCODE_W]) {
        move_camera_forward(&(render_context.camera), speed);
    }
    if (keystate[SDL_SCANCODE_S]) {
        move_camera_backward(&(render_context.camera), speed);
    }
    if (keystate[SDL_SCANCODE_A]) {
        move_camera_left(&(render_context.camera), speed);
    }
    if (keystate[SDL_SCANCODE_D]) {
        move_camera_right(&(render_context.camera), speed);
    }
}

