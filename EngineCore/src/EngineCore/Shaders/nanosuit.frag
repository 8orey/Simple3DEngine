#version 430

const uint MAX_POINT_LIGHT_ARRAY_SIZE = 32;

struct Fragment {
    vec3 position_eye;
    vec3 normal_eye;
    vec2 texture_position;
};

struct Material {
    sampler2D diffuse0;
    sampler2D specular0;
    float shininess;
};

struct PointLight {
    vec3 position_eye;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    float linear;
    float quadro;
    float intensity;
};

struct PointLightArray {
    uint size;
    PointLight pnts[MAX_POINT_LIGHT_ARRAY_SIZE];
};

struct texture_t {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 normal;
};

in Fragment frag;
out vec4 fragment_color;

uniform Material material;
uniform PointLightArray PLA;

const texture_t text = texture_t(
    texture(material.diffuse0, frag.texture_position).rgb,
    texture(material.diffuse0, frag.texture_position).rgb,
    texture(material.specular0, frag.texture_position).rgb,
    normalize(frag.normal_eye)
);

vec3 calc_light(const PointLight light);

void main() {
    vec3 res = {0.f, 0.f, 0.f};

    for (uint i = 0; i < PLA.size; ++i) {
        res += calc_light(PLA.pnts[i]);
    }
        
    fragment_color = vec4(res, 1.f);
}

vec3 calc_light(const PointLight light) {
    vec3 light_dir = normalize(light.position_eye - frag.position_eye);
    
    // Коэффицент яркости освещения с изменением расстояния 
    float dist = length(frag.position_eye - light.position_eye);
    float attenuation = light.intensity / (1.0f + dist * (light.linear + light.quadro * dist));

    // Пассивный фактор 
    vec3 ambient = light.ambient * text.ambient;
    // Фактор прямого освещения 
    float dif = max(dot(text.normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * text.diffuse * dif;
    // Фактор блика 
    vec3 view_dir = normalize(-frag.position_eye);
    vec3 reflect_dir = reflect(-light_dir, text.normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = text.specular * specular_value * light.specular;

    return (specular + diffuse + ambient) * attenuation;
}


