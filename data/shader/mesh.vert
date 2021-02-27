#version 330 core

layout (location = 0) in uvec3 vert_position;
layout (location = 1) in ivec3 vert_normal;
layout (location = 2) in uint vert_material;

void main() {
	float voxel_sz = 1.0;

	gl_Position = vec4(vec3(vert_position) * voxel_sz, 1.0);
}