#version 410 core

layout (location = 0) out vec4 color;
layout (location = 0) in vec2 v_world_pos;

uniform vec2 caster_min;
uniform vec2 caster_max;

void main() {
    if (v_world_pos.x >= caster_min.x && v_world_pos.x <= caster_max.x &&
        v_world_pos.y >= caster_min.y && v_world_pos.y <= caster_max.y) {
        discard;
    }
    color = vec4(1.0, 1.0, 1.0, 1.0); // White
}