#version 330 core

#define PI 3.1415926535897932384626433832795

in vec2 frag_uv;

out vec4 frag_color;

uniform sampler2D albedo_tex;
uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D ssao_tex;

uniform vec3 sky_color;
uniform float z_far;

uniform mat4 view;

const vec3 world_light_dir = normalize(vec3(-0.7, 1.5, 0.5));

void main() {
    vec3 albedo = texture(albedo_tex, frag_uv).rgb;
    vec3 position = texture(position_tex, frag_uv).xyz;
    vec3 normal = texture(normal_tex, frag_uv).xyz;
    float ambient_occlusion = texture(ssao_tex, frag_uv).r;

    vec3 lighting = albedo * ambient_occlusion * 0.3f;
    vec3 light_dir = normalize(mat3(view) * world_light_dir);
            
    vec3 diffuse = max(dot(normal, light_dir), 0.0f) * albedo * ambient_occlusion;
    lighting += diffuse;

    if (normal.x == 0.0f && normal.y == 0.0f && normal.z == 0.0f) {
        // Sky
        lighting = albedo;
    }

    // Fog
    float depth = min(1.0f, length(position) / z_far);
    float fog = depth * depth;
    frag_color = vec4(mix(lighting, sky_color, fog), 1.0);
}