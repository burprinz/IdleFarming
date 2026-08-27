#version 410 core

layout (location = 0) in vec2 frag_uv;

uniform sampler2D sprite;
uniform vec3 fcolor;

layout (location = 0) out vec4 color;

void main() {
    vec4 texColor = texture(sprite, frag_uv);

    if (texColor.a < 0.01) {
        discard;
    }

    color = vec4(fcolor, 1.0) * texColor;
}