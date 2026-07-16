#include "basileus/mesh.h"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#include <glad/gl.h>

#include <stdbool.h>
#include <stdlib.h>


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

    unsigned int vertex_length = 0;
    for (int i = 0; i < (int)layout->attribute_count; i++) {
        VertexAttribute attribute = layout->attributes[i];
        vertex_length += attribute.size;
    }
    printf("vertex_length: %d\n", vertex_length);
    
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m->vertices[0]) * m->vertex_count * vertex_length, 
            m->vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m->indices[0]) * m->index_count, 
            m->indices, GL_STATIC_DRAW);

    /*unsigned int vertex_length = 0;
    for (int i = 0; i < (int)layout->attribute_count; i++) {
        VertexAttribute attribute = layout->attributes[i];
        vertex_length += attribute.size;
    }*/

    unsigned int offset = 0;
    for (int i = 0; i < (int)layout->attribute_count; i++) {
        VertexAttribute attribute = layout->attributes[i];

        glVertexAttribPointer(attribute.location, attribute.size, GL_FLOAT, 
                GL_FALSE, vertex_length * sizeof(float), 
                (void*)(offset * sizeof(float))); 

        offset += attribute.size;
        glEnableVertexAttribArray(attribute.location);
    }
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

Mesh *load_gltf_file(const char *file_path, int *mesh_count) {
    cgltf_options options = {0};
    cgltf_data *data = NULL;

    // Parse the actual .gltf file
    if (cgltf_parse_file(&options, file_path, &data) != cgltf_result_success) {
        fprintf(stderr, "Failed to parse .gltf file '%s'\n", file_path);
        *mesh_count = 0;
        return NULL;
    }

    // Load in all of the .gltf file's associated binary data
    if (cgltf_load_buffers(&options, data, file_path) != cgltf_result_success) {
        fprintf(stderr, "Failed to load .gltf file '%s' buffers", file_path);
        cgltf_free(data);
        *mesh_count = 0;
        return NULL;
    }

    // Check that the .gltf file's data is valid
    // TODO: just free data and return NULL here?
    if (cgltf_validate(data) != cgltf_result_success) {
        fprintf(stderr, "Warning: file '%s' may be malformed!\n", file_path);
    }

    // Get the total number of primitives (draw calls) in the .gltf file
    int total_primitives = 0;
    for (cgltf_size i = 0; i < data->meshes_count; i++) {
        total_primitives += (int)(data->meshes[i].primitives_count);
    }

    // Upload each primitive to the GPU and store their meshes in the final list
    Mesh *mesh_list = malloc(total_primitives * sizeof(Mesh));
    int mesh_index = 0;

    for (int i = 0; i < (int)data->meshes_count; i++) {
        cgltf_mesh *mesh = &data->meshes[i];

        for (int j = 0; j < (int)mesh->primitives_count; j++) {
            cgltf_primitive *primitive = &mesh->primitives[j];

            // Skip meshes that aren't made out of triangles
            if (primitive->type != cgltf_primitive_type_triangles) {
                continue;
            }

            mesh_list[mesh_index] = upload_primitive(primitive);
            mesh_index++;
        }
    }

    *mesh_count = mesh_index;

    cgltf_free(data);
    return mesh_list;
}

Mesh upload_primitive(const cgltf_primitive *primitive) {
    Mesh mesh = {0};

    float *positions = NULL;
    float *normals = NULL;
    float *tex_coords = NULL;
    cgltf_size vertex_count = 0;
    cgltf_size vertex_size = 0;

    // Get the vertex attributes in the given primitive
    for (int i = 0; i < (int)primitive->attributes_count; i++) {
        const cgltf_attribute *attribute = &primitive->attributes[i];

        if (attribute->type == cgltf_attribute_type_position) {
            positions = read_float_accessor(attribute->data, &vertex_count,
                                            &vertex_size);
        }
        if (attribute->type == cgltf_attribute_type_normal) {
            normals = read_float_accessor(attribute->data, &vertex_count,
                                          &vertex_size);
        }
        if (attribute->type == cgltf_attribute_type_texcoord) {
            tex_coords = read_float_accessor(attribute->data, &vertex_count,
                                             &vertex_size);
        }
    }

    // Interleave each vertex attribute into a single array of vertex data
    float *vertices = malloc(vertex_count * vertex_size * sizeof(float));

    for (int i = 0; i < (int)vertex_count; i++) {
        float *curr_float = &vertices[i * vertex_size];
        int offset = 0;

        if (positions) {
            memcpy(curr_float + offset, &positions[i * 3], sizeof(float) * 3);
            offset += 3;
        }
        if (normals) {
            memcpy(curr_float + offset, &normals[i * 3], sizeof(float) * 3);
            offset += 3;
        }
        if (tex_coords) {
            memcpy(curr_float + offset, &tex_coords[i * 2], sizeof(float) * 2);
            offset += 2;
        }
    }

    // Get the indices in the given primitive
    unsigned int *indices = NULL;
    cgltf_size index_count = 0;
    if (primitive->indices) {
        indices = read_index_accessor(primitive->indices, &index_count);
    }

    // Create a proper VertexLayout struct
    int attribute_count = 0;
    if (positions) {
        attribute_count++;
    }
    if (normals) {
        attribute_count++;
    }
    if (tex_coords) {
        attribute_count++;
    }

    int attribute_index = 0;
    VertexAttribute *vertex_attributes = malloc(attribute_count * sizeof(VertexAttribute));

    if (positions) {
        vertex_attributes[attribute_index] = create_vertex_attribute(0, 3);
        attribute_index++;
    }
    if (normals) {
        vertex_attributes[attribute_index] = create_vertex_attribute(1, 3);
        attribute_index++;
    }
    if (tex_coords) {
        vertex_attributes[attribute_index] = create_vertex_attribute(2, 2);
        attribute_index++;
    }

    VertexLayout vertex_layout = create_vertex_layout(vertex_attributes, attribute_count);

    // Fill out the actual Mesh struct
    mesh = create_mesh(vertices, (size_t)vertex_count, indices, (size_t)index_count);
    upload_mesh(&mesh, &vertex_layout);

    // DEBUG STUFF--DELETE LATER!
    /*printf("vertex_count: %d\n", (int)vertex_count);
    printf("index_count: %d\n", (int)index_count);
    printf("vertex_size: %d\n\n", (int)vertex_size);

    printf("First 10 vertices:\n");
    for (int i = 0; i < 80; i++) {
        printf("%.2f ", vertices[i]);
    }
    printf("\n");

    printf("First 80 indices:\n");
    for (int i = 0; i < 80; i++) {
        printf("%d ", indices[i]);
    }
    printf("\n\n");*/

    // Cleanup and return
    free(vertex_attributes);

    if (positions) {
        free(positions);
    }
    if (normals) {
        free(normals);
    }
    if (tex_coords) {
        free(tex_coords);
    }

    free(vertices);
    free(indices);

    return mesh;
}

float *read_float_accessor(cgltf_accessor *accessor, cgltf_size *vertex_count,
                           cgltf_size *vertex_size) {
    cgltf_size num_vertices = accessor->count;
    cgltf_size num_components = cgltf_num_components(accessor->type);

    float *vertex_data = malloc(num_vertices * num_components * sizeof(float));
    cgltf_accessor_unpack_floats(accessor, vertex_data, num_vertices * num_components);

    // Overwriting vertex_count is OK, since each vertex component must have
    // the same count
    *vertex_count = num_vertices;
    *vertex_size += num_components;

    return vertex_data;
}

unsigned int *read_index_accessor(cgltf_accessor *accessor,
                                  cgltf_size *index_count) {
    cgltf_size num_indices = accessor->count;

    unsigned int *index_data = malloc(num_indices * sizeof(unsigned int));
    cgltf_accessor_unpack_indices(accessor, index_data, sizeof(unsigned int),
                                  num_indices);

    *index_count = num_indices;

    return index_data;
}


