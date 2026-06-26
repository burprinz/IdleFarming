#version 410 core
in vec2 v_worldPos;
out vec4 FragColor;

uniform vec3  color;
uniform vec2  beam_start;
uniform vec2  beam_end;
uniform float half_width;
uniform float core_ratio;

float distToSegment(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    float ab2 = dot(ab, ab);
    if (ab2 < 1e-8) return length(p - a);
    float t = clamp(dot(p - a, ab) / ab2, 0.0, 1.0);
    vec2 proj = a + t * ab;
    return length(p - proj);
}

void main() {
    float d = distToSegment(v_worldPos, beam_start, beam_end);
    float x = d / max(half_width, 1e-6);

    float edgeAlpha = 0.01;
    float k = -log(edgeAlpha);
    float alpha = exp(-k * x * x);

    FragColor = vec4(color, alpha);

}
