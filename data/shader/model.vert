#version 330 core

layout (location = 0) in vec3 vert_position;
layout (location = 1) in vec3 vert_normal;
layout (location = 2) in uint vert_material;

out vec3 frag_position;
out vec3 frag_normal;
flat out uint frag_material;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	vec4 view_position = view * model * vec4(vert_position, 1.0f);
    frag_position = view_position.xyz;
    gl_Position = proj * view_position;
    mat3 normal_matrix = transpose(inverse(mat3(view * model)));
    frag_normal = normal_matrix * vert_normal;
	frag_material = vert_material;
}