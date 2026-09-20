#version 330 core

// localCoord.x in [-1, 1] quer zum Objekt
// localCoord.y in [-1, 1]: -1 = Unterkante/Basis des Objekts, 1 = Spitze des Schimmer-Quads
in vec2 localCoord;

out vec4 fragColor;

// 0.0 = Kreis, 1.0 = Quadrat/Rechteck (als float, damit keine setInt-Abhängigkeit im Shader-Wrapper nötig ist)
uniform float shapeMode;
uniform float intensity;   // 0..1, Ein-/Ausblenden beim Hover
uniform float time;
uniform vec3 glowColor;

float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

void main() {
    vec2 p = localCoord;

    // normierte Höhe: 0 = Basis (am Objekt), 1 = Spitze des Schimmers
    float h = clamp((p.y + 1.0) * 0.5, 0.0, 1.0);

    // Strahlenbündel verjüngt sich leicht nach oben
    float taper = mix(1.0, 0.35, h);
    float x = p.x / max(taper, 0.001);

    // Silhouette an der Basis: rund oder eckig
    float baseShape;
    if (shapeMode < 0.5) {
        baseShape = 1.0 - smoothstep(0.7, 1.0, length(vec2(x, p.y)));
    } else {
        baseShape = 1.0 - smoothstep(0.7, 1.0, max(abs(x), abs(p.y)));
    }
    baseShape *= (1.0 - h); // nur unten relevant, oben übernehmen die Strahlen

    // horizontales und vertikales Ausbleichen des Bündels
    float horizontalFalloff = 1.0 - smoothstep(0.0, 1.0, abs(x));
    float verticalFalloff = pow(1.0 - h, 1.3);

    // prozedurale, ungleichmäßig verteilte "Sonnenstrahlen" (nur nach oben, kein Ring)
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

    // Helligkeitswelle, die kontinuierlich nach oben "wandert"
    float travel = fract(h * 1.5 - time * 0.5);
    float travelBand = smoothstep(0.0, 0.25, travel) * smoothstep(0.65, 0.25, travel);

    float glow = max(baseShape * 0.6, rayPattern * horizontalFalloff) * verticalFalloff;
    glow = mix(glow, glow * (0.5 + travelBand), 0.6);

    float alpha = glow * intensity;
    fragColor = vec4(glowColor, alpha);
}
