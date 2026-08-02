#version 410 core

layout (location = 0) in vec2 frag_uv;

uniform sampler2D sprite;
uniform vec3 fcolor;

layout (location = 0) out vec4 color;

void main() {
    vec4 texColor = texture(sprite, frag_uv);

    //weiße Pixel irgnorieren, schwarz bleibt schwarz
    if (texColor.r > 0.99f && texColor.g > 0.99f && texColor.b > 0.99f) {
        discard;
    }
    //nur nicht weiße Teile rendern
    color = vec4(fcolor, 1.0) * texColor;
}