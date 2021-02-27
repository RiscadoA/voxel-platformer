#version 330 core

in vec2 frag_uv;

out float frag_color;
        
const int NUM_SAMPLES = 64;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D noise_tex;

uniform mat4 projection;
uniform vec2 noise_scale;
uniform vec3 samples[NUM_SAMPLES];

const float radius = 0.5;
const float bias = 0.025;
const float magnitude = 1.1;
const float constrast = 1.1;

void main() {
    vec3 frag_pos = texture(position_tex, frag_uv).xyz;
    vec3 normal = normalize(texture(normal_tex, frag_uv).xyz);
    vec3 random_vec = normalize(texture(noise_tex, frag_uv * noise_scale).xyz);

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);
            
    float occlusion = 0.0;
    for (int i = 0; i < NUM_SAMPLES; ++i) {
        vec3 sample_pos = tbn * samples[i];
        sample_pos = frag_pos + sample_pos * radius;
                
        vec4 offset = projection * vec4(sample_pos, 1.0);
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;
                
        float sample_depth = texture(position_tex, offset.xy).z;
        float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;
    }
            
    occlusion = 1.0 - occlusion / NUM_SAMPLES;
    occlusion = pow(occlusion, magnitude);
    frag_color = constrast * (occlusion - 0.5) + 0.5;
}