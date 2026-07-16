#ifndef MESH_H
#define MESH_H

#include <cgltf.h>

#include <stdlib.h>


typedef struct {
    float *vertices;
    size_t vertex_count;

    unsigned int *indices;
    size_t index_count;

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


Mesh create_mesh(float *vert, size_t vert_count, unsigned int *ind, 
        size_t ind_count);

void upload_mesh(Mesh *m, VertexLayout *layout);

VertexAttribute create_vertex_attribute(unsigned int location, 
        unsigned int size);

VertexLayout create_vertex_layout(VertexAttribute *attributes, 
        size_t attribute_count);

void cleanup_mesh(Mesh *m);

Mesh *load_gltf_file(const char *file_path, int *mesh_count);

Mesh upload_primitive(const cgltf_primitive *primtive);

float *read_float_accessor(cgltf_accessor *accessor, cgltf_size *vertex_count,
                           cgltf_size *vertex_size);

unsigned int *read_index_accessor(cgltf_accessor *accessor,
                                  cgltf_size *index_count);

#endif

