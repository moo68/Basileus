#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal_vec;
out vec3 frag_pos;
out vec2 tex_coords;

void main() { 
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    normal_vec = normalMatrix * normal;

    frag_pos = vec3(model * vec4(pos, 1.0));

    tex_coords = uv;

    gl_Position = projection * view * model * vec4(pos, 1.0); 
}

