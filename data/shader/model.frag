#version 330 core

struct Material {
   vec3 color;
   float emissive;
};

layout (std140) uniform Palette {
   Material palette[255];
};

in vec3 frag_position;
in vec3 frag_normal;
flat in uint frag_material;

layout (location = 0) out vec4 albedo;
layout (location = 1) out vec3 position;
layout (location = 2) out vec3 normal;

void main() {
	Material mat = palette[frag_material];
	albedo.rgb = mat.color;
	albedo.a = mat.emissive;
	position = frag_position;
	normal = normalize(frag_normal);
}