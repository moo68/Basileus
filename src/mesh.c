#include "basileus/mesh.h"

#include <stdlib.h>

#include <glad/gl.h>


Mesh create_mesh(float *vert, size_t vert_count, unsigned int *ind, 
        size_t ind_count) {
    Mesh m;
    m.vertices = vert;
    m.vertex_count = vert_count;
    m.indices = ind;
    m.index_count = ind_count;

    m.vbo = 0;
    m.ebo = 0;
    m.vao = 0;

    return m;
}

void upload_mesh(Mesh *m, VertexLayout *layout) {
    glGenVertexArrays(1, &m->vao);
    glGenBuffers(1, &m->vbo);
    glGenBuffers(1, &m->ebo);
    
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m->vertices[0]) * m->vertex_count, 
            m->vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m->indices[0]) * m->index_count, 
            m->indices, GL_STATIC_DRAW);

    unsigned int vertex_length = 0;
    for (int i = 0; i < (int)layout->attribute_count; i++) {
        VertexAttribute attribute = layout->attributes[i];
        vertex_length += attribute.size;
    }

    unsigned int offset = 0;
    for (int i = 0; i < (int)layout->attribute_count; i++) {
        VertexAttribute attribute = layout->attributes[i];

        glVertexAttribPointer(attribute.location, attribute.size, GL_FLOAT, 
                GL_FALSE, vertex_length * sizeof(float), 
                (void*)(offset * sizeof(float))); 

        offset += attribute.size;
        glEnableVertexAttribArray(attribute.location);
    }

    //glEnableVertexAttribArray(0);
}

VertexAttribute create_vertex_attribute(unsigned int location, 
        unsigned int size) {
    VertexAttribute va;
    va.location = location;
    va.size = size; 

    return va;
}
 
VertexLayout create_vertex_layout(VertexAttribute *attributes, 
        size_t attribute_count) {
    VertexLayout vl;
    vl.attributes = attributes;
    vl.attribute_count = attribute_count;

    return vl;
}

void cleanup_mesh(Mesh *m) {
    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ebo);
}

