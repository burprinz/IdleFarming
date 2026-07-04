#version 410 core
in vec2 vLocalPos;
out vec4 FragColor;

uniform vec2 halfSize;   // halbe Breite/Höhe in Weltkoordinaten
uniform float radius;    // Eckenradius in Weltkoordinaten
uniform float thickness; // 0 = gefüllt, >0 = Outline-Dicke
uniform vec3 color;
uniform float opacity;

float roundedBoxSDF(vec2 p, vec2 halfSize, float radius) {
    vec2 q = abs(p) - (halfSize - radius);
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

void main() {
    vec2 p = vLocalPos * halfSize;
    float dist = roundedBoxSDF(p, halfSize, radius);
    float aa = fwidth(dist); // Anti-Aliasing über 1 Pixel Breite

    float alpha;
    if (thickness <= 0.0) {
        alpha = 1.0 - smoothstep(-aa, aa, dist);
    } else {
        float outer = 1.0 - smoothstep(-aa, aa, dist);
        float inner = 1.0 - smoothstep(-aa, aa, dist + thickness);
        alpha = outer - inner;
    }

    if (alpha <= 0.001) discard;
    FragColor = vec4(color, alpha * opacity);
}