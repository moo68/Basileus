#include <stdio.h> 
#include <stdbool.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include "basileus/mesh.h"
#include "basileus/camera.h"
#include "basileus/material.h"
#include "basileus/shader_utils.h"
#include "basileus/texture_utils.h"
#include "basileus/debug_geometry.h"


GLFWwindow *create_window_and_context(const int width, const int height, const char *name);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void process_input(GLFWwindow *window);


bool is_wireframe = false;

Camera camera;
bool first_mouse = true;
float last_x =  1280.0f / 2.0;
float last_y =  720.0 / 2.0;

float delta_time = 0.0f;
float last_frame = 0.0f;


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

    // Data    
    Cube cube = generate_cube(); // TODO: Debug geometry should probably make
                                 // meshes, not its own type with redundant data?
    Mesh cube_mesh = create_mesh(cube.vertices, 144, cube.indices, 36);

    // Buffers
    VertexAttribute position = create_vertex_attribute(0, 3);
    VertexAttribute normal = create_vertex_attribute(1, 3);
    VertexAttribute pos_attributes[] = {position, normal};
    VertexLayout vertex_pos_layout = create_vertex_layout(pos_attributes, 2); 

    upload_mesh(&cube_mesh, &vertex_pos_layout); 

    // Textures 
    //unsigned int texture = load_texture("assets/textures/bricks.jpg"); 

    // Camera
    camera = create_camera(0.0f, 0.0f, 3.0f, 45.0f, 0.1f);

    mat4 projection, view, model;

    glm_mat4_identity(projection);
    glm_perspective(glm_rad(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 
                0.1f, 100.0f, projection);
   
    Material object = create_material(shader_program);
    Material light_source = create_material(light_source_shader);

    vec3 color, light, light_pos;
    glm_vec3_copy((vec3){1.0f, 0.5f, 0.31f}, color);
    glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, light); 
    glm_vec3_copy((vec3){3.0f, 1.0f, -3.0f}, light_pos);
    
    // Render Loop
    while (!glfwWindowShouldClose(window)) {
        float current_frame = (float)(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        set_view_matrix(&camera, &view);

        glBindVertexArray(cube_mesh.vao);
        glUseProgram(object.shader_program);

        glUniform3fv(object.color_loc, 1, (float *)color);
        glUniform3fv(object.light_loc, 1, (float *)light);
        glUniform3fv(object.light_pos_loc, 1, (float *)light_pos);
        glUniform3fv(object.view_pos_loc, 1, (float *)camera.position);

        glUniformMatrix4fv(object.view_loc, 1, GL_FALSE, (float *)view);
        glUniformMatrix4fv(object.projection_loc, 1, GL_FALSE, (float *)projection);

        glm_mat4_identity(model);
        glm_translate(model, (vec3){-1.0f, 0.0f, 0.0f});
        glUniformMatrix4fv(object.model_loc, 1, GL_FALSE, (float *)model);
            
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 

        glBindVertexArray(cube_mesh.vao);
        glUseProgram(light_source.shader_program);
  
        glUniformMatrix4fv(light_source.view_loc, 1, GL_FALSE, (float *)view);
        glUniformMatrix4fv(light_source.projection_loc, 1, GL_FALSE, (float *)projection);
 
        glm_mat4_identity(model); 
        glm_vec3_rotate(light_pos, delta_time * 0.5f, (vec3){0.0f, 1.0f, 1.0f});
        glm_translate(model, light_pos); 
        glm_scale(model, (vec3){0.25f, 0.25f, 0.25f});
        glUniformMatrix4fv(light_source.model_loc, 1, GL_FALSE, (float *)model);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

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

    look_camera_around(&camera, xpos, ypos, &last_x, &last_y, &first_mouse); 
}

void process_input(GLFWwindow *window) {
    (void)window;

    const float speed = 2.5f * delta_time; 

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        move_camera_forward(&camera, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        move_camera_backward(&camera, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        move_camera_left(&camera, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        move_camera_right(&camera, speed);
    }
}

