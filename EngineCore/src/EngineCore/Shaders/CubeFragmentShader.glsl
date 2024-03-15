#version 430


struct Fragment {
    vec3 position_eye;
    vec3 normal_eye;
    vec2 texture_position;
};

in Fragment frag;

out vec4 frag_color;

struct Material {
    sampler2D ambient0;
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

    float constant;
    float linear;
    float quadro;
    float intensity;
};

uniform Material material;

vec3 texture_ambient = texture(material.ambient0, frag.texture_position).rgb;
vec3 texture_diffuse = texture(material.diffuse0, frag.texture_position).rgb;
vec3 texture_specular = texture(material.specular0, frag.texture_position).rgb;
vec3 frag_normal = normalize(frag.normal_eye);

const int CNT_MAX_POINT_LIGHTS = 110;
uniform int cnt_pointLights;
uniform PointLight pointLights[CNT_MAX_POINT_LIGHTS];

vec3 CalcPointLight(const PointLight light);

void main() {
    vec3 res = vec3(0.f, 0.f, 0.f);
    for (int i = 0; i < cnt_pointLights; ++i) {
        res += CalcPointLight(pointLights[i]);
    }
    frag_color = vec4(res, 1.f);
}

vec3 CalcPointLight(const PointLight light) {
    vec3 light_dir = normalize(light.position_eye - frag.position_eye);
    
    // Коэффицент яркости освещения с изменением расстояния 
    float dist = length(frag.position_eye - light.position_eye);
    float attenuation = 1.0 / (
        light.constant + 
        light.linear * dist +
        light.quadro * dist * dist
    );

    // Пассивный фактор 
    vec3 ambient = light.ambient * texture_ambient;
    // Фактор прямого освещения 
    float dif = max(dot(frag_normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * texture_diffuse * dif;

    // Фактор блика 
    vec3 view_dir = normalize(-frag.position_eye);
    vec3 reflect_dir = reflect(-light_dir, frag_normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = texture_specular * specular_value * light.specular;

    return (specular + diffuse + ambient) * light.intensity;
}
