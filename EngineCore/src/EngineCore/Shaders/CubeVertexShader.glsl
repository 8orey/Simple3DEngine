#version 430

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal_vec;
layout(location = 2) in vec2 texture_coord;

uniform mat4 module_view_matrix;
uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;
        
out vec3 frag_normal_eye;
out vec3 frag_pos_eye;
out vec2 tex_pos;

void main() {
    tex_pos = texture_coord;
	frag_normal_eye = normal_matrix * normal_vec;
    frag_pos_eye = vec3(module_view_matrix * vec4(vertex_position, 1.0f));
    gl_Position = mvp_matrix * vec4(vertex_position, 1.0f);
}