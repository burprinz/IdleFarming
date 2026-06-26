#version 410 core
layout (location = 0) in vec4 vert;
out vec2 TexPos;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vert.xy, -1.0, 1.0);
    TexPos = vert.zw;
}