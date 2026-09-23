#version 330 core

in vec2 localCoord;

out vec4 fragColor;

uniform float shapeMode;
uniform float intensity;
uniform float time;
uniform vec3 glowColor;

float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

void main() {
    vec2 p = localCoord;

    float h = clamp((p.y + 1.0) * 0.5, 0.0, 1.0);
    float verticalFalloff = pow(1.0 - h, 0.9);

    vec2 origin = vec2(0.0, -1.0);
    vec2 v = p - origin;
    float dist = length(v);
    float angle = atan(v.x, v.y);

    // WINKEL ZUM ANPASSEN
    const float MAX_ANGLE = 1.4;

    float rayPattern = 0.0;
    const int RAYS = 24;
    for (int i = 0; i < RAYS; i++) {
        float seed = float(i) * 1.37;
        float t = (float(i) + 0.5) / float(RAYS);
        float rayAngle = mix(-MAX_ANGLE, MAX_ANGLE, t);
        float rayWidth = 0.1 + hash(seed + 5.0) * 0.05;
        float speed = 0.3 + hash(seed + 9.0) * 0.5;
        float flicker = 0.6 + 0.4 * sin(time * speed * 3.0 + seed * 10.0);

        float sinA = max(abs(sin(rayAngle)), 0.05);
        float maxDistForAngle = 0.95 / sinA;
        float fadeEnd = min(maxDistForAngle, 1.4);
        float rayFade = smoothstep(0.05, 0.15, dist) * (1.0 - smoothstep(fadeEnd * 0.6, fadeEnd, dist));

        float d = abs(angle - rayAngle);
        rayPattern += smoothstep(rayWidth, 0.0, d) * flicker * rayFade;
    }
    rayPattern = clamp(rayPattern, 0.0, 1.0);

    float travel = fract(h * 1.5 - time * 0.5);
    float travelBand = smoothstep(0.0, 0.25, travel) * smoothstep(0.65, 0.25, travel);

    float glow = rayPattern * verticalFalloff;
    glow = mix(glow, glow * (0.5 + travelBand), 0.6);

    float alpha = glow * intensity;
    fragColor = vec4(glowColor, alpha);
}