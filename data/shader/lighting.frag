#version 330 core

in vec2 frag_uvs;

out vec4 frag_color;

void main() {
	frag_color = vec4(frag_uvs, 0.0, 1.0);
}