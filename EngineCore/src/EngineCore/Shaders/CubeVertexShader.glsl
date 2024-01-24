#version 430

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal_vec;
layout(location = 2) in vec2 texture_coord;

uniform mat4 module_view_matrix;
uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;
        
struct Fragment {
    vec3 position_eye;
    vec3 normal_eye;
    vec2 texture_position;
};

out Fragment frag;

void main() {
    frag.texture_position = texture_coord;
	frag.normal_eye = normal_matrix * normal_vec;
    frag.position_eye = vec3(module_view_matrix * vec4(vertex_position, 1.0f));
    gl_Position = mvp_matrix * vec4(vertex_position, 1.0f);
}