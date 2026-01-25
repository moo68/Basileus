#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal_vec;
out vec3 frag_pos;

void main() { 
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    normal_vec = normalMatrix * normal;

    frag_pos = vec3(model * vec4(pos, 1.0));

    gl_Position = projection * view * model * vec4(pos, 1.0); 
}

