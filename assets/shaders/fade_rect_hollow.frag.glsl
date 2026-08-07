#version 410 core
in vec2 vLocalCoord; // Range [-1.0, 1.0]

uniform vec2 halfSize;
uniform vec3 outer_color;
uniform float fading_speed;
uniform float opacity;

out vec4 FragColor;

void main() {
    vec2 p = abs(vLocalCoord * halfSize);

    vec2 distToEdge = halfSize - p;

    float minEdgeDist = min(distToEdge.x, distToEdge.y);

    float fadeRange = 0.5 * min(halfSize.x, halfSize.y);

    float inward = clamp(minEdgeDist / fadeRange, 0.0, 1.0);

    float factor = pow(1.0 - inward, max(fading_speed, 0.0001));

    FragColor = vec4(outer_color, factor * opacity);
}