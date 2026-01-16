#include "basileus/shader_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/gl.h>


char *read_shader_file(const char* filepath) {
    char *shader_source = NULL;
    size_t total_length = 0;

    FILE *fp = fopen(filepath, "rb");
    if (fp == NULL) {
        fprintf(stderr, "ERROR! Could not open shader file: %s\n", filepath);
        return NULL;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp) != NULL) {
        size_t line_length = strlen(line);

        char *new_buffer = realloc(shader_source, total_length + line_length + 1);
        if (!new_buffer) {
            fprintf(stderr, "ERROR! Could not reallocate shader file buffer!");
            free(shader_source);
            fclose(fp);
            return NULL;
        }

        shader_source = new_buffer;
        memcpy(shader_source + total_length, line, line_length);

        total_length += line_length;
        shader_source[total_length] = '\0';
    }

    fclose(fp);
    return shader_source;
}

unsigned int create_shader(const GLchar *shader_source, GLenum shader_type) {
    unsigned int shader;
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "ERROR! Vertex shader compilation failed:\n%s\n", info_log); 
    }

    return shader;
}

unsigned int create_shader_program(const GLchar *vertex_source, const GLchar *fragment_source) {
    const char *vertex_shader_source = read_shader_file(vertex_source);
    if (vertex_shader_source == NULL) {
        fprintf(stderr, "ERROR! Failed to read vertex shader!");
    }
    const char *fragment_shader_source = read_shader_file(fragment_source); 
    if (fragment_shader_source == NULL) {
        fprintf(stderr, "ERROR! Failed to read fragment shader!"); 
    }

    unsigned int vertex_shader = create_shader(vertex_shader_source, GL_VERTEX_SHADER);
    unsigned int fragment_shader = create_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    int success;
    char info_log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        fprintf(stderr, "ERROR! Shader linking failed:\n%s\n", info_log); 
    } 

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

