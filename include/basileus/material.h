#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/gl.h>


typedef struct {
    GLuint shader_program;

    GLint model_loc;
    GLint view_loc;
    GLint projection_loc;

    GLint color_loc;
    GLint light_loc;
    GLint light_pos_loc;
    GLint view_pos_loc;
} Material;



Material create_material(GLuint shader_program);

#endif

