#include "basileus/material.h"

#include <glad/gl.h>


Material create_material(GLuint shader_program) {
    Material mat;
    mat.shader_program = shader_program;

    mat.model_loc = glGetUniformLocation(shader_program, "model");
    mat.view_loc = glGetUniformLocation(shader_program, "view");
    mat.projection_loc = glGetUniformLocation(shader_program, "projection");

    mat.color_loc = glGetUniformLocation(shader_program, "object_color");
    mat.light_loc = glGetUniformLocation(shader_program, "light_color");

    return mat;
}

