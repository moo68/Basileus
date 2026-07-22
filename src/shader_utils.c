#include "basileus/shader_utils.h"
#include "basileus/renderer.h"
#include "basileus/material.h"

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

unsigned int compile_shader_file(const GLchar *shader_source, GLenum shader_type) {
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

unsigned int create_shader_program(const GLchar *vertex_source, 
        const GLchar *fragment_source) {
    const char *vertex_shader_source = read_shader_file(vertex_source);
    if (vertex_shader_source == NULL) {
        fprintf(stderr, "ERROR! Failed to read vertex shader!");
    }
    const char *fragment_shader_source = read_shader_file(fragment_source); 
    if (fragment_shader_source == NULL) {
        fprintf(stderr, "ERROR! Failed to read fragment shader!"); 
    }

    unsigned int vertex_shader = compile_shader_file(vertex_shader_source, 
            GL_VERTEX_SHADER);
    unsigned int fragment_shader = compile_shader_file(fragment_shader_source, 
            GL_FRAGMENT_SHADER);

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

/*SimpleShader *create_simple_shader(unsigned int shader_program) {
    SimpleShader *s = malloc(sizeof(SimpleShader));
    
    s->base.shader_program = shader_program;
    s->base.upload_uniforms = upload_simple_uniforms;

    s->model_loc = glGetUniformLocation(shader_program, "model");
    s->view_loc = glGetUniformLocation(shader_program, "view");
    s->projection_loc = glGetUniformLocation(shader_program, "projection");

    return s;
}

void upload_simple_uniforms(RenderContext *context, RenderObject *object) {
    SimpleShader *shader = (SimpleShader *)object->shader;

    glUniformMatrix4fv(shader->view_loc, 1, GL_FALSE, (float *)context->camera.view);
    glUniformMatrix4fv(shader->projection_loc, 1, GL_FALSE, (float *)context->projection);
    glUniformMatrix4fv(shader->model_loc, 1, GL_FALSE, (float *)object->transform.model);
}*/

PhongShader *create_phong_shader(unsigned int shader_program) { 
    PhongShader *s = malloc(sizeof(PhongShader));

    s->base.shader_program = shader_program;
    s->base.upload_uniforms = upload_phong_uniforms;

    s->model_loc = glGetUniformLocation(shader_program, "model");
    s->view_loc = glGetUniformLocation(shader_program, "view");
    s->projection_loc = glGetUniformLocation(shader_program, "projection");

    s->ambient_loc = glGetUniformLocation(shader_program, "material.ambient");
    s->diffuse_loc = glGetUniformLocation(shader_program, "material.diffuse");
    s->specular_loc = glGetUniformLocation(shader_program, "material.specular");
    s->shininess_loc = glGetUniformLocation(shader_program, "material.shininess");

    s->ambient_light_loc = glGetUniformLocation(shader_program, "light.ambient");
    s->diffuse_light_loc = glGetUniformLocation(shader_program, "light.diffuse");
    s->specular_light_loc = glGetUniformLocation(shader_program, "light.specular");
    s->light_pos_loc = glGetUniformLocation(shader_program, "light.position");

    s->view_pos_loc = glGetUniformLocation(shader_program, "view_position");

    return s;
}

void upload_phong_uniforms(RenderContext *context, Renderable *render,
                           Transform *transform) {
    PhongShader *shader = (PhongShader *)render->shader;

    glUniformMatrix4fv(shader->view_loc, 1, GL_FALSE, (float *)context->camera.view);
    glUniformMatrix4fv(shader->projection_loc, 1, GL_FALSE, (float *)context->projection);
    glUniformMatrix4fv(shader->model_loc, 1, GL_FALSE, (float *)transform->model);

    PhongMaterial *material = (PhongMaterial *)render->material;

    glUniform3fv(shader->ambient_loc, 1, (float *)material->ambient);
    glUniform3fv(shader->diffuse_loc, 1, (float *)material->diffuse);
    glUniform3fv(shader->specular_loc, 1, (float *)material->specular);
    glUniform1f(shader->shininess_loc, material->shininess);

    glUniform3fv(shader->ambient_light_loc, 1, (float *)context->directional_light.ambient);
    glUniform3fv(shader->specular_light_loc, 1, (float *)context->directional_light.specular);
    glUniform3fv(shader->diffuse_light_loc, 1, (float *)context->directional_light.diffuse);
    //glUniform3fv(shader->light_pos_loc, 1, (float *)context->directional_light.position);
    glUniform3fv(shader->light_pos_loc, 1, (float *)context->directional_light.direction);

    glUniform3fv(shader->view_pos_loc, 1, (float *)context->camera.position);
}

/*TexturedPhongShader *create_textured_phong_shader(unsigned int shader_program) {
    TexturedPhongShader *s = malloc(sizeof(TexturedPhongShader));

    s->base.shader_program = shader_program;
    s->base.upload_uniforms = upload_textured_phong_uniforms;

    s->model_loc = glGetUniformLocation(shader_program, "model");
    s->view_loc = glGetUniformLocation(shader_program, "view");
    s->projection_loc = glGetUniformLocation(shader_program, "projection");

    s->diffuse_tex_loc = glGetUniformLocation(shader_program, "material.diffuse");
    s->specular_tex_loc = glGetUniformLocation(shader_program, "material.specular");
    s->shininess_loc = glGetUniformLocation(shader_program, "material.shininess");

    s->ambient_light_loc = glGetUniformLocation(shader_program, "dir_light.ambient");
    s->diffuse_light_loc = glGetUniformLocation(shader_program, "dir_light.diffuse");
    s->specular_light_loc = glGetUniformLocation(shader_program, "dir_light.specular");
    s->light_dir_loc = glGetUniformLocation(shader_program, "dir_light.direction");

    s->num_point_lights_loc = glGetUniformLocation(shader_program, "num_point_lights");

    s->view_pos_loc = glGetUniformLocation(shader_program, "view_position");

    return s;
}

void upload_textured_phong_uniforms(RenderContext *context, RenderObject *object) {
    TexturedPhongShader *shader = (TexturedPhongShader *)object->shader;

    glUniformMatrix4fv(shader->view_loc, 1, GL_FALSE, (float *)context->camera.view);
    glUniformMatrix4fv(shader->projection_loc, 1, GL_FALSE, (float *)context->projection);
    glUniformMatrix4fv(shader->model_loc, 1, GL_FALSE, (float *)object->transform.model);

    TexturedPhongMaterial *material = (TexturedPhongMaterial *)object->material;

    glUniform1f(shader->shininess_loc, material->shininess);

    glUniform3fv(shader->ambient_light_loc, 1, (float *)context->directional_light.ambient);
    glUniform3fv(shader->specular_light_loc, 1, (float *)context->directional_light.specular);
    glUniform3fv(shader->diffuse_light_loc, 1, (float *)context->directional_light.diffuse);
    glUniform3fv(shader->light_dir_loc, 1, (float *)context->directional_light.direction);

    // Set point light values. Note that this gets the location and sets its
    // value every frame; uniform locations here aren't being cached. Because 
    // of that (among other things) this is nasty and should be replaced by UBOs.
    glUniform1i(shader->num_point_lights_loc, context->num_point_lights);

    char uniform_name[32];
    for (int i = 0; i < context->num_point_lights; i++) {
        snprintf(uniform_name, sizeof(uniform_name), "point_lights[%d].position", i);
        GLint position_loc = glGetUniformLocation(shader->base.shader_program, uniform_name);
        glUniform3fv(position_loc, 1, (float *)context->point_lights[i].position);

        snprintf(uniform_name, sizeof(uniform_name), "point_lights[%d].ambient", i);
        GLint ambient_loc = glGetUniformLocation(shader->base.shader_program, uniform_name);
        glUniform3fv(ambient_loc, 1, (float *)context->point_lights[i].ambient);

        snprintf(uniform_name, sizeof(uniform_name), "point_lights[%d].diffuse", i);
        GLint diffuse_loc = glGetUniformLocation(shader->base.shader_program, uniform_name);
        glUniform3fv(diffuse_loc, 1, (float *)context->point_lights[i].diffuse);

        snprintf(uniform_name, sizeof(uniform_name), "point_lights[%d].specular", i);
        GLint specular_loc = glGetUniformLocation(shader->base.shader_program, uniform_name);
        glUniform3fv(specular_loc, 1, (float *)context->point_lights[i].specular);

        snprintf(uniform_name, sizeof(uniform_name), "point_lights[%d].constant", i);
        GLint constant_loc = glGetUniformLocation(shader->base.shader_program, uniform_name);
        glUniform1f(constant_loc, context->point_lights[i].constant);

        snprintf(uniform_name, sizeof(uniform_name), "point_lights[%d].linear", i);
        GLint linear_loc = glGetUniformLocation(shader->base.shader_program, uniform_name);
        glUniform1f(linear_loc, context->point_lights[i].linear);

        snprintf(uniform_name, sizeof(uniform_name), "point_lights[%d].quadratic", i);
        GLint quadratic_loc = glGetUniformLocation(shader->base.shader_program, uniform_name);
        glUniform1f(quadratic_loc, context->point_lights[i].quadratic);
    }

    glUniform3fv(shader->view_pos_loc, 1, (float *)context->camera.position);

    // Set textures' texture units 
    glUniform1i(shader->diffuse_tex_loc, 0);
    glUniform1i(shader->specular_tex_loc, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->diffuse_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material->specular_tex);
}*/

