#version 410 core

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;

in vec3 normal_vec;
in vec3 frag_pos;

out vec4 frag_color;

void main() {
    float ambient_strength = 0.3;
    vec3 ambient = ambient_strength * light_color;

    vec3 norm = normalize(normal_vec);
    vec3 light_dir = normalize(light_position - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    float specular_strength = 0.5;
    vec3 view_dir = normalize(view_position - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;  

    vec3 result = (ambient + diffuse + specular) * object_color;
    frag_color = vec4(result, 1.0);
}

