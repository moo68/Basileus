#version 410 core

in vec3 vertex_color;
in vec2 texture_coordinate;
out vec4 frag_color;

uniform sampler2D the_texture;

void main() {
    frag_color = texture(the_texture, texture_coordinate) * vec4(vertex_color, 1.0);
}

