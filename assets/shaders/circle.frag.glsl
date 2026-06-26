#version 410 core

in vec2 vPos;

uniform vec3 fcolor;
uniform float opacity;
uniform float radius;

out vec4 FragColor;

void main() {
    float dist = length(vPos);

    if (dist > radius) {
        discard;
    }

    float smoothening = 1.0 - smoothstep(0.95, 1.0, dist);

    FragColor = vec4(fcolor, opacity);
}