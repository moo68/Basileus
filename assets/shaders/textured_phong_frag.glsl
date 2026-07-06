#version 410 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    //vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform DirLight dir_light;
uniform vec3 view_position;

in vec3 normal_vec;
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 frag_color;

void main() {
    vec3 ambient = dir_light.ambient * vec3(texture(material.diffuse, tex_coords));

    vec3 norm = normalize(normal_vec);
    //vec3 light_dir = normalize(light.position - frag_pos);
    vec3 light_dir = normalize(-dir_light.direction);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = dir_light.diffuse * diff * vec3(texture(material.diffuse, tex_coords));

    vec3 view_dir = normalize(view_position - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = dir_light.specular * spec * vec3(texture(material.specular, tex_coords));

    vec3 result = (ambient + diffuse + specular);
    frag_color = vec4(result, 1.0);
}

