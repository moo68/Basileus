#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <stdlib.h>
#include <glad/gl.h>


typedef struct RenderObject RenderObject;
typedef struct RenderContext RenderContext;

typedef struct Shader {
    GLuint shader_program;
    void (*upload_uniforms)(RenderContext *context, RenderObject *object);
} Shader;

typedef struct {
    Shader base;

    GLint model_loc;
    GLint view_loc;
    GLint projection_loc;
} SimpleShader;

typedef struct {
    Shader base;

    GLint model_loc;
    GLint view_loc;
    GLint projection_loc;

    // Phong material attributes.
    GLint ambient_loc;
    GLint diffuse_loc;
    GLint specular_loc;
    GLint shininess_loc;

    // Phong light attributes.
    GLint ambient_light_loc;
    GLint diffuse_light_loc;
    GLint specular_light_loc;
    GLint light_pos_loc;

    GLint view_pos_loc;
} PhongShader;

typedef struct {
    Shader base;

    GLint model_loc;
    GLint view_loc;
    GLint projection_loc;

    // Phong material attributes.
    GLint ambient_loc;
    GLint diffuse_loc;
    GLint specular_loc;
    GLint shininess_loc;

    // Phong light attributes.
    GLint ambient_light_loc;
    GLint diffuse_light_loc;
    GLint specular_light_loc;
    GLint light_pos_loc;

    GLint view_pos_loc;
} TexturedPhongShader;


char *read_shader_file(const char* filepath);

unsigned int compile_shader_file(const GLchar *shader_source, GLenum shader_type);

unsigned int create_shader_program(const GLchar *vertex_source, const GLchar *fragment_source);

SimpleShader *create_simple_shader(unsigned int shader_program);

void upload_simple_uniforms(RenderContext *context, RenderObject *object);

PhongShader *create_phong_shader(unsigned int shader_program);

void upload_phong_uniforms(RenderContext *context, RenderObject *object);

#endif

