#version 330 core

const vec2[6] positions = vec2[6](
	vec2(-1.0, -1.0),
	vec2(+1.0, -1.0),
	vec2(+1.0, +1.0),
	vec2(+1.0, +1.0),
	vec2(-1.0, +1.0),
	vec2(-1.0, -1.0)
);

const vec2[6] uvs = vec2[6](
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, 0.0)
);

out vec2 frag_uv;

void main() {
	frag_uv = uvs[gl_VertexID];
	gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}