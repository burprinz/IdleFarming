#version 410 core
layout (location = 0) in vec2 worldPos;
layout (location = 1) in vec2 localPos;

uniform mat4 view;
uniform mat4 projection;

out vec2 vLocalPos;

void main() {
    vLocalPos = localPos;
    gl_Position = projection * view * vec4(worldPos, 0.0, 1.0);
}