#version 410 core

layout (location = 0) in vec2 position;
layout (location = 0) out vec2 v_world_pos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    v_world_pos = position;
    gl_Position = projection * view * vec4(position, 0.0, 1.0);
}