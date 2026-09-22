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

    // Breitenprofil: Maximum jetzt exakt am Äquator (t=0.5), nicht mehr an der Oberkante
    float taper;
    if (h < 0.5) {
        float t = h / 0.5; // 0 (Bodenspitze) .. 1 (Kreis-Oberkante, läuft dort wieder auf einen Punkt zu)
        taper = (shapeMode < 0.5)
        ? 2.0 * sqrt(max(t * (1.0 - t), 0.0)) // echtes Kreisprofil: 0 unten, Maximum bei t=0.5, 0 oben
        : 1.0;                                 // Quadrat bleibt überall gleich breit
    } else {
        float t = (h - 0.5) / 0.5;
        taper = mix(0.0, 0.35, t); // Übergang von der Kreisspitze ins Strahlenbündel
    }
    float x = p.x / max(taper, 0.02);

    float baseShape;
    if (shapeMode < 0.5) {
        baseShape = 1.0 - smoothstep(0.7, 1.0, length(vec2(x, p.y)));
    } else {
        baseShape = 1.0 - smoothstep(0.7, 1.0, max(abs(x), abs(p.y)));
    }
    baseShape *= (1.0 - h);

    float horizontalFalloff = 1.0 - smoothstep(0.0, 1.0, abs(x));
    float verticalFalloff = pow(1.0 - h, 1.3);

    float rayPattern = 0.0;
    const int RAYS = 7;
    for (int i = 0; i < RAYS; i++) {
        float seed = float(i) * 1.37;
        float center = hash(seed) * 2.0 - 1.0;
        float width = 0.05 + hash(seed + 5.0) * 0.05;
        float speed = 0.3 + hash(seed + 9.0) * 0.5;
        float flicker = 0.6 + 0.4 * sin(time * speed * 3.0 + seed * 10.0);
        float d = abs(x - center);
        rayPattern += smoothstep(width, 0.0, d) * flicker;
    }
    rayPattern = clamp(rayPattern, 0.0, 1.0);

    float travel = fract(h * 1.5 - time * 0.5);
    float travelBand = smoothstep(0.0, 0.25, travel) * smoothstep(0.65, 0.25, travel);

    float glow = max(baseShape * 0.6, rayPattern * horizontalFalloff) * verticalFalloff;
    glow = mix(glow, glow * (0.5 + travelBand), 0.6);

    float alpha = glow * intensity;
    fragColor = vec4(glowColor, alpha);
}