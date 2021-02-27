#version 330 core

struct Material {
   vec3 color;
   float _padding;
};

layout (std140) uniform Palette {
   Material palette[255];
};

flat in uint frag_material;

out vec4 frag_color;

void main() {
	Material mat = palette[frag_material];
	frag_color = vec4(mat.color, 1.0);
}