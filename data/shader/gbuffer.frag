#version 330 core

#define PI 3.1415926535897932384626433832795
#define LIGHT_COUNT 64

in vec2 frag_uv;

out vec4 frag_color;

uniform sampler2D albedo_tex;
uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D ssao_tex;

uniform mat4 proj;
uniform mat4 view;
uniform float z_far;

struct Light {
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    float constant;
    float linear;
    float quadratic;
    float _padding;
};

layout (std140) uniform Lights {
   Light lights[LIGHT_COUNT];
};

void main() {
    vec4 albedo = texture(albedo_tex, frag_uv);
    vec3 position = texture(position_tex, frag_uv).xyz;
    vec3 normal = texture(normal_tex, frag_uv).xyz;
    float ambient_occlusion = texture(ssao_tex, frag_uv).r;

    vec3 lighting;

    for (int i = 0; i < LIGHT_COUNT; ++i) {
        vec3 ambient = albedo.rgb * ambient_occlusion * lights[i].ambient.xyz;
        if (lights[i].direction.w > 0.5f) {
            // Point light
            vec3 light_dir = normalize(lights[i].position.xyz - position);
            float dist = length(lights[i].position.xyz - position);

            vec3 diffuse = max(dot(normal, light_dir), 0.0f) * lights[i].diffuse.xyz * albedo.rgb * ambient_occlusion;
            float attenuation = 1.0 / (lights[i].constant + lights[i].linear * dist + lights[i].quadratic * (dist * dist));    
            lighting += (ambient + diffuse) * attenuation;
        }
        else {
            // Directional Light
            vec3 light_dir = lights[i].direction.xyz;
            vec3 diffuse = max(dot(normal, light_dir), 0.0f) * lights[i].diffuse.xyz * albedo.rgb * ambient_occlusion;
            lighting += ambient + diffuse;
        }
    }

    lighting = clamp(lighting, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
    
    if (albedo.a > 0.0f) {
        lighting = albedo.rgb;
    }

    // Sky
    vec3 sky_dir = normalize(inverse(mat3(view)) * (inverse(proj) * vec4(frag_uv * 2.0f - 1.0f, 0.0f, 1.0f)).xyz);
    vec3 sky_color = mix(vec3(0.6, 0.6, 0.8), vec3(0.0, 0.0, 1.0), clamp(sky_dir.y / 1.5f + 0.0f, 0.0, 1.0));
    //vec3 sky_color = mix(vec3(0.1, 0.4, 0.6), vec3(0.2, 0.6, 0.9), clamp(sky_dir.y / 1.5f + 0.5f, 0.0, 1.0));
    
    if (normal.x == 0.0f && normal.y == 0.0f && normal.z == 0.0f) {
        lighting = sky_color;
    }

    // Fog
    float depth = min(1.0f, length(position) / z_far);
    float fog = depth * depth * depth;
    frag_color = vec4(mix(lighting, sky_color, fog), 1.0);
}