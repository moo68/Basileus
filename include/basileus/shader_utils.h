#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <stdlib.h>

#include <glad/gl.h>


typedef struct {
    GLuint shader_program;

    GLint model_loc;
    GLint view_loc;
    GLint projection_loc;
} BasicShader;

typedef struct {
    GLuint shader_program;

    GLint model_loc;
    GLint view_loc;
    GLint projection_loc;

    GLint color_loc;
    GLint light_loc;
    GLint light_pos_loc;
    GLint view_pos_loc;
} PhongShader;


char *read_shader_file(const char* filepath);

unsigned int compile_shader_file(const GLchar *shader_source, GLenum shader_type);

unsigned int create_shader_program(const GLchar *vertex_source, const GLchar *fragment_source);

BasicShader create_basic_shader(unsigned int shader_program);

PhongShader create_phong_shader(unsigned int shader_program);

#endif

