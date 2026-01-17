#include <stdio.h>
#include <stdbool.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include "basileus/shader_utils.h"
#include "basileus/texture_utils.h"


GLFWwindow *create_window_and_context(const int width, const int height, const char *name);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void process_input(GLFWwindow *window);


bool is_wireframe = false;

// camera
vec3 cameraPos   = { 0.0f, 0.0f, 3.0f };
vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
vec3 cameraUp    = { 0.0f, 1.0f, 0.0f };

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


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
    unsigned int shader_program = create_shader_program("assets/shaders/vertex.glsl",
            "assets/shaders/fragment.glsl");

    // Data--this is currently gross! Will implement a simple cube generator function! 
    float vertices[] = {
    // Position          // Color           // Texture
    // Front face
     0.5f,  0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // top right
     0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // top left

    // Back face
    -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,

    // Left face
    -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,

    // Right face
     0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,

    // Top face
     0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,

    // Bottom face
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
    // Front face
    0, 1, 3,
    1, 2, 3,
    // Back face
    4, 5, 7,
    5, 6, 7,
    // Left face
    8, 9, 11,
    9,10,11,
    // Right face
   12,13,15,
   13,14,15,
    // Top face
   16,17,19,
   17,18,19,
    // Bottom face
   20,21,23,
   21,22,23
    };
        

    // Buffers
    unsigned int vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Textures 
    unsigned int texture = load_texture("assets/textures/bricks.jpg"); 

    // Projection matrix
    mat4 projection;
    glm_mat4_identity(projection);
    glm_perspective(glm_rad(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 
                0.1f, 100.0f, projection);
   
    // Render Loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        // Matrix math
        mat4 model, view;
        glm_mat4_identity(model);
        glm_mat4_identity(view);

        glm_rotate(model, glm_rad(-55.0f), (vec3){1.0f, 0.0f, 0.0f});
        glm_rotate(model, glm_rad(glfwGetTime() * 100), (vec3){0.0f, 1.0f, 1.0f});
 
        vec3 target;
        glm_vec3_add(cameraPos, cameraFront, target);
        glm_lookat(cameraPos, target, cameraUp, view); 

        glUseProgram(shader_program);

        // Upload matrix math
        unsigned int modelLoc = glGetUniformLocation(shader_program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
        unsigned int viewLoc = glGetUniformLocation(shader_program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
        unsigned int projectionLoc = glGetUniformLocation(shader_program, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *)projection);

        glBindVertexArray(vao); 
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Shutdown cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shader_program);
        
    glfwTerminate();
    return 0;
}


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
    glViewport(0, 0, width, height);

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

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    vec3 front;
    front[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
    front[1] = sin(glm_rad(pitch));
    front[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
    glm_vec3_norm(front);
    glm_vec3_copy(front, cameraFront);
}

void process_input(GLFWwindow *window) {
    (void)window;

    const float cameraSpeed = 2.5f * deltaTime;
    vec3 velocity;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_scale(cameraFront, cameraSpeed, velocity);
        glm_vec3_add(cameraPos, velocity, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_scale(cameraFront, cameraSpeed, velocity);
        glm_vec3_sub(cameraPos, velocity, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 left;
        glm_vec3_cross(cameraFront, cameraUp, left);
        glm_vec3_norm(left);
        glm_vec3_scale(left, cameraSpeed, velocity);
        glm_vec3_sub(cameraPos, velocity, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 right;
        glm_vec3_cross(cameraFront, cameraUp, right);
        glm_vec3_norm(right);
        glm_vec3_scale(right, cameraSpeed, velocity);
        glm_vec3_add(cameraPos, velocity, cameraPos);
    }
}

