#include <stdio.h> 
#include <stdbool.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
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


GLFWwindow *create_window_and_context(const int width, const int height, const char *name);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void process_input(GLFWwindow *window);


RenderContext render_context = {0};

bool is_wireframe = false;

bool first_mouse = true;
float last_x =  1280.0f / 2.0;
float last_y =  720.0 / 2.0;


int main(void) { 
    // Load GLFW and OpenGL
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    GLFWwindow *window = create_window_and_context(WINDOW_WIDTH, WINDOW_HEIGHT, "Basileus");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback); 
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Shaders
    /*unsigned int shader_program = create_shader_program("assets/shaders/vertex.glsl",
            "assets/shaders/fragment.glsl");*/
    unsigned int shader_program = create_shader_program("assets/shaders/debug_vertex.glsl",
            "assets/shaders/debug_fragment.glsl");
    unsigned int light_source_shader = create_shader_program("assets/shaders/lighting_vertex.glsl",
            "assets/shaders/lighting_fragment.glsl");
 
    PhongShader *phong_shader = create_phong_shader(shader_program);
    SimpleShader *simple_shader = create_simple_shader(light_source_shader);

    // Meshes and Buffers
    Cube cube = generate_cube();
    Mesh cube_mesh = create_mesh(cube.vertices, 144, cube.indices, 36);

    VertexAttribute position = create_vertex_attribute(0, 3);
    VertexAttribute normal = create_vertex_attribute(1, 3);
    VertexAttribute pos_attributes[] = {position, normal};
    VertexLayout vertex_pos_layout = create_vertex_layout(pos_attributes, 2); 

    upload_mesh(&cube_mesh, &vertex_pos_layout); 

    // Textures 
    //unsigned int texture = load_texture("assets/textures/bricks.jpg"); 

    // Render context 
    Camera camera = create_camera(0.0f, 0.0f, 3.0f, 45.0f, 0.1f);
    render_context.camera = camera;

    glm_mat4_identity(render_context.projection);
    glm_perspective(glm_rad(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 
                0.1f, 100.0f, render_context.projection);
 
    glm_vec3_copy((vec3){3.0f, 1.0f, -3.0f}, render_context.light_position);

    // Materials
    vec3 ambient, diffuse, specular;
    glm_vec3_copy((vec3){1.0f, 0.5f, 0.31f}, ambient); 
    glm_vec3_copy((vec3){1.0f, 0.5f, 0.31f}, diffuse); 
    glm_vec3_copy((vec3){0.5f, 0.5f, 0.5f}, specular); 
    float shininess = 32.0f;
    PhongMaterial *phong_mat = create_phong_material(ambient, diffuse, specular, shininess);
 
    // Transforms
    Transform object_transform = create_transform();
    translate_transform(&object_transform, (vec3){-1.0f, 0.0f, 0.0f});

    Transform light_transform = create_transform();
    translate_transform(&light_transform, render_context.light_position);
    scale_transform(&light_transform, (vec3){0.25f, 0.25f, 0.25f});

    // RenderObjects
    RenderObject phong_cube = {
        .mesh = &cube_mesh,
        .transform = object_transform,
        .shader = (Shader *)phong_shader,
        .material = phong_mat
    };
 
    RenderObject light_cube = {
        .mesh = &cube_mesh,
        .transform = light_transform,
        .shader = (Shader *)simple_shader,
        .material = NULL
    };

    RenderObject render_objects[2] = {phong_cube, light_cube};
    int num_render_objects = 2;
    
    // Render Loop
    while (!glfwWindowShouldClose(window)) {
        float current_frame = (float)(glfwGetTime());
        render_context.delta_time = current_frame - render_context.last_frame;
        render_context.last_frame = current_frame;

        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        set_view_matrix(&(render_context.camera));

        for (int i = 0; i < num_render_objects; i++) {
            RenderObject curr_object = render_objects[i];

            glBindVertexArray(curr_object.mesh->vao);

            glUseProgram(curr_object.shader->shader_program);

            curr_object.shader->upload_uniforms(&render_context, &curr_object);

            glDrawElements(GL_TRIANGLES, curr_object.mesh->index_count, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Shutdown cleanup
    cleanup_mesh(&cube_mesh);
    glDeleteProgram(shader_program);
    glDeleteProgram(light_source_shader);
        
    glfwTerminate();
    return 0;
}


// Function definitions
GLFWwindow* create_window_and_context(const int width, const int height, const char *name) {
    if (!glfwInit()) {
        printf("ERROR: GLFW failed to initialize!\n");
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
 
    GLFWwindow *window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: GLFW failed to create a window!\n");
        glfwTerminate();
        return NULL;
    } 
    glfwMakeContextCurrent(window);
 
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        fprintf(stderr, "ERROR: GLAD failed to initialize!\n");
        return NULL;
    }
    else {
        fprintf(stdout, "INFO: Loaded OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), 
                GLAD_VERSION_MINOR(version));
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height); 
    glViewport(0, 0, fb_width, fb_height);

    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;

    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } 
    else if (!is_wireframe && key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
        is_wireframe = true;
    }
    else if (is_wireframe && key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
        is_wireframe = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window; 

    look_camera_around(&(render_context.camera), xpos, ypos, &last_x, &last_y, &first_mouse); 
}

void process_input(GLFWwindow *window) {
    (void)window;

    const float speed = 2.5f * render_context.delta_time; 

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        move_camera_forward(&(render_context.camera), speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        move_camera_backward(&(render_context.camera), speed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        move_camera_left(&(render_context.camera), speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        move_camera_right(&(render_context.camera), speed);
    }
}

