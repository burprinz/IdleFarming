#version 410 core

layout (location = 0) in vec2 frag_uv;
layout (location = 0) out vec4 color;

uniform sampler2D screen_texture;
uniform sampler2D beam_shadow_texture;

uniform float screen_darkness;
uniform float world_width;
uniform float world_height;
uniform vec2  view_min;
uniform float time;

uniform int   beam_count;
uniform vec2  beam_start[32];
uniform vec2  beam_end[32];
uniform vec3  beam_color[32];
uniform float beam_light_radius;
uniform float beam_light_strength;

float distToSegment(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    float ab2 = dot(ab, ab);
    if (ab2 < 1e-8) return length(p - a);
    float t = clamp(dot(p - a, ab) / ab2, 0.0, 1.0);
    vec2 proj = a + t * ab;
    return length(p - proj);
}

float beamLightAt(vec2 p) {
    float best = 0.0;

    float edgeAlpha = 0.02;
    float k = -log(edgeAlpha);

    for (int i = 0; i < beam_count; i++) {
        float d = distToSegment(p, beam_start[i], beam_end[i]);
        float x = d / max(beam_light_radius, 1e-6);
        float a = exp(-k * x * x);
        best = max(best, a);
    }

    return beam_light_strength * best;
}

vec3 beamColorAt(vec2 p) {
    vec3 final_color = vec3(0.0, 0.0, 0.0);

    float edgeAlpha = 0.02;
    float k = -log(edgeAlpha);

    for (int i = 0; i < beam_count; i++) {
        float d = distToSegment(p, beam_start[i], beam_end[i]);
        float x = d / max(beam_light_radius, 1e-6);
        float a = exp(-k * x * x);

        float w = a * beam_light_strength;
        final_color += beam_color[i] * w;
    }

    return final_color;
}

void main() {
    vec4 base_color = texture(screen_texture, frag_uv);
    float shadow_mask = texture(beam_shadow_texture, frag_uv).r;

    vec2 pixel_world_pos = view_min + vec2(frag_uv.x * world_width, frag_uv.y * world_height);

    float beam_light = beamLightAt(pixel_world_pos);
    vec3 beam_col = beamColorAt(pixel_world_pos);

    beam_light *= (1.0 - shadow_mask);

    float light_factor = clamp(beam_light, 0.0, 1.0);

    vec3 lit_color = base_color.rgb * (1.0 + beam_col * light_factor);

    color = vec4(lit_color, light_factor);
}
