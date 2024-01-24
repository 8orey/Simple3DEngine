#version 430

#define CNT_POINT_LIGHTS 2
#define CNT_DIR_LIGHTS 0

struct Fragment {
    vec3 position_eye;
    vec3 normal_eye;
    vec2 texture_position;
};

in Fragment frag;

out vec4 frag_color;

struct Material {
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
};

struct DirLight {
    vec3 position_eye;
    vec3 direction_eye;

    float cutOff;
    float OcutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadro;
    float intensity;
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

vec3 texture_ambient = texture(material.ambient, frag.texture_position).rgb;
vec3 texture_diffuse = texture(material.diffuse, frag.texture_position).rgb;
vec3 texture_specular = texture(material.specular, frag.texture_position).rgb;
vec3 frag_normal = normalize(frag.normal_eye);

uniform PointLight pointLights[CNT_POINT_LIGHTS];
// uniform DirLight dirLights[CNT_DIR_LIGHTS];

vec3 CalcDirLight(DirLight light);
vec3 CalcPointLight(PointLight light);

void main() {
    vec3 res = vec3(0.f, 0.f, 0.f);

    // for (int i = 0; i < CNT_DIR_LIGHTS; ++i) {
    //    res += CalcDirLight(dirLights[i]);
    //}
    for (int i = 0; i < CNT_POINT_LIGHTS; ++i) {
        res += CalcPointLight(pointLights[i]);
    }

    frag_color = vec4(res, 1.f);
}

vec3 CalcDirLight(DirLight light) {
    vec3 light_dir = normalize(light.position_eye - frag.position_eye);

    float theta = dot(light_dir, normalize(-light.direction_eye));
    float epsilon = light.cutOff - light.OcutOff;
    float intensity = clamp((theta - light.OcutOff) / epsilon, 0.0, 1.0);

    if (theta <= light.cutOff) {
        return light.ambient * texture_ambient;
    }

    // dist 
    float dist = length(frag.position_eye - light.position_eye);
    float attenuation = 1.0 / (
        light.constant + 
        light.linear * dist +
        light.quadro * dist * dist
    );

    // ambient
    vec3 ambient = light.ambient * texture_ambient;

    // diffuse
    float dif = max(dot(frag_normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * texture_diffuse * dif;

    // specular
    vec3 view_dir = normalize(-frag.position_eye);
    vec3 reflect_dir = reflect(-light_dir, frag_normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = texture_specular * specular_value * light.specular;

    return (specular + diffuse + ambient) * light.intensity * intensity;
}

vec3 CalcPointLight(PointLight light) {
    vec3 light_dir = normalize(light.position_eye - frag.position_eye);
    
    // dist 
    float dist = length(frag.position_eye - light.position_eye);
    float attenuation = 1.0 / (
        light.constant + 
        light.linear * dist +
        light.quadro * dist * dist
    );

    // ambient
    vec3 ambient = light.ambient * texture_ambient;

    // diffuse
    float dif = max(dot(frag_normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * texture_diffuse * dif;

    // specular
    vec3 view_dir = normalize(-frag.position_eye);
    vec3 reflect_dir = reflect(-light_dir, frag_normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = texture_specular * specular_value * light.specular;

    return (specular + diffuse + ambient) * light.intensity;
}
