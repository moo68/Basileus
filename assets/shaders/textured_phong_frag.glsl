#version 410 core

#define MAX_POINT_LIGHTS (32)


struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};


uniform PointLight point_lights[MAX_POINT_LIGHTS];

uniform Material material;
uniform DirLight dir_light;
uniform vec3 view_position;

in vec3 normal_vec;
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 frag_color;


vec3 calc_dir_light(DirLight dir_light, vec3 normal, vec3 view_dir) {
    vec3 light_dir = normalize(-dir_light.direction);

    // Ambient
    vec3 ambient = dir_light.ambient * vec3(texture(material.diffuse, tex_coords));

    // Diffuse
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = dir_light.diffuse * diff * vec3(texture(material.diffuse, tex_coords));

    // Specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = dir_light.specular * spec * vec3(texture(material.specular, tex_coords));

    // Final result
    vec3 result = (ambient + diffuse + specular);
    return result;
}

void main() {
    // Properties
    vec3 normal = normalize(normal_vec);
    vec3 view_dir = normalize(view_position - frag_pos);
    vec3 result = vec3(0.0);

    // Directional lighting
    result = calc_dir_light(dir_light, normal, view_dir);

    // Final result
    frag_color = vec4(result, 1.0);
}

