#ifndef DEBUG_GEOMETRY_H
#define DEBUG_GEOMETRY_H

typedef struct {
    float vertices[144];
    unsigned int indices[36];
} Cube;

typedef struct {
    float vertices[192];
    unsigned int indices[36];
} TexturedCube;


Cube generate_cube();

TexturedCube generate_textured_cube();

#endif

