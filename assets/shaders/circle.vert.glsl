#version 410 core
layout(location = 0) in vec2 pos;

out vec2 vPos;

uniform mat4 view;
uniform mat4 projection;
uniform vec2 center;

void main() {
    vPos = pos;
    gl_Position = projection * view * vec4(center+pos, 0.0, 1.0);
}

