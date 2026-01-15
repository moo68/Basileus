#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdbool.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


int main() {
    GLFWwindow* window;
    if (!glfwInit()) {
        printf("ERROR: GLFW failed to initialize!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Basileus", NULL, NULL);
    if (!window) {
        printf("ERROR: GLFW failed to create a window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("ERROR: GLAD failed to initialize!\n");
        return -1;
    }
    else {
        printf("INFO: Loaded OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(version), 
                GLAD_VERSION_MINOR(version));
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
        
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

