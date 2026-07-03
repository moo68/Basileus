#version 410 core

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 view_position;

in vec3 normal_vec;
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 frag_color;

void main() {
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords));

    vec3 norm = normalize(normal_vec);
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords));

    vec3 view_dir = normalize(view_position - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = (ambient + diffuse + specular);
    frag_color = vec4(result, 1.0);
}

