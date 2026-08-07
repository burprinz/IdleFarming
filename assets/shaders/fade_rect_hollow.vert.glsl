#version 410 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 localCoord;

uniform mat4 view;
uniform mat4 projection;

out vec2 vLocalCoord;

void main() {
    vLocalCoord = localCoord;
    gl_Position = projection * view * vec4(pos, 0.0, 1.0);
}