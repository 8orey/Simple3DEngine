#version 430

in vec3 frag_normal_eye;
in vec3 frag_pos_eye;
in vec2 tex_pos;

layout(binding = 0) uniform sampler2D ourTexture;   

uniform float ambient_factor;
uniform float diffuse_factor;
uniform float specular_factor;
uniform float shine_factor;

uniform vec3 light_color;
uniform vec3 light_pos_eye;

out vec4 frag_color;

void main() {
   
    
    // ambient
    vec3 ambient = ambient_factor * light_color;
    
    // diffuse
    vec3 normal = normalize(frag_normal_eye);
    vec3 light_dir = normalize(light_pos_eye - frag_pos_eye);
    vec3 diffuse = diffuse_factor * light_color * max(dot(normal, light_dir), 0.0f);

    // specular
    vec3 view_dir = normalize(-frag_pos_eye);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), shine_factor);
    vec3 specular = specular_factor * specular_value * light_color;

    frag_color = texture(ourTexture, tex_pos) * vec4(ambient + diffuse + specular, 1.f);
}