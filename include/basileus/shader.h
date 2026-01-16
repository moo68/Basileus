#ifndef SHADER_H
#define SHADER_H

#include <stdlib.h>

#include <glad/gl.h>


char *read_shader_file(char* filepath);
unsigned int create_shader(const GLchar *shader_source, GLenum shader_type);
unsigned int create_shader_program(unsigned int shader_list[], size_t length);

#endif

