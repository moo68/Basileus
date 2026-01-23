#ifndef MESH_H
#define MESH_H

#include <stdlib.h>


typedef struct {
    float *vertices;
    size_t vertices_length;

    unsigned int *indices;
    size_t indices_length;

    unsigned int vbo, ebo, vao; 
} Mesh;

typedef struct {
    unsigned int location;
    unsigned int size; 
} VertexAttribute;

typedef struct {
    VertexAttribute *attributes;
    size_t attribute_count;
} VertexLayout;


Mesh create_mesh(float *vert, size_t vert_length, unsigned int *ind, 
        size_t ind_length);

void upload_mesh(Mesh *m, VertexLayout *layout);

VertexAttribute create_vertex_attribute(unsigned int location, 
        unsigned int size);

VertexLayout create_vertex_layout(VertexAttribute *attributes, 
        size_t attribute_count);

#endif

