#version 410 core

layout (location = 0) in vec2 frag_uv;
layout (location = 0) out vec4 color;

uniform sampler2D screen_texture;
uniform sampler2D shadow_texture;

uniform vec2  player_position;
uniform float illumination_radius;
uniform float screen_darkness;
uniform float world_width;
uniform float world_height;
uniform float time;


void main() {
    vec4 base_color = texture(screen_texture, frag_uv);
    float shadow_mask = texture(shadow_texture, frag_uv).r;

    vec2 pixel_world_pos = vec2(frag_uv.x * world_width, frag_uv.y * world_height);

    // Player light (unchanged)
    float dist = length(pixel_world_pos - player_position);

    float pulse = sin(time * 3.0) * 0.05 + 1.0;
    float effective_radius = illumination_radius * pulse;

    float player_light = 1.0 - smoothstep(0.0, effective_radius, dist);

    float flicker = 0.97 + fract(sin(time * 50.0) * 43758.5453) * 0.03;
    player_light *= flicker;


    player_light *= (1.0 - shadow_mask);

    float light_factor = clamp(player_light, 0.0, 1.0);

    float final_brightness = mix(1.0 - screen_darkness, 1.0, light_factor);
    vec3 lit_color = base_color.rgb * final_brightness;

    color = vec4(lit_color, base_color.a);
}
