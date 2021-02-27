#version 330 core

layout (location = 0) in vec3 vert_position;
layout (location = 1) in vec3 vert_normal;
layout (location = 2) in uint vert_material;

flat out uint frag_material;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	frag_material = vert_material;
	gl_Position = proj * view * model * vec4(vert_position, 1.0);
}