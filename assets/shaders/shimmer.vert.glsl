#version 330 core

// layout: pos.xy (world space) + localCoord.xy (-1..1), analog zu round_rect/fade_rect
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aLocal;

uniform mat4 view;
uniform mat4 projection;

out vec2 localCoord;

void main() {
    localCoord = aLocal;
    gl_Position = projection * view * vec4(aPos, 0.0, 1.0);
}
