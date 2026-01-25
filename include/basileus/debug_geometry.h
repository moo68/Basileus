#ifndef DEBUG_GEOMETRY_H
#define DEBUG_GEOMETRY_H

typedef struct {
    float vertices[144];
    unsigned int indices[36];
} Cube;


Cube generate_cube();

#endif

