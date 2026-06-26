#version 410 core

layout (location = 0) in vec2 frag_uv;
layout (location = 0) out vec4 outColor;

uniform sampler2D light_texture;

// uv-coordinates for frame A and B
uniform vec4 uvA;
uniform vec4 uvB;
uniform float t;    // interpolation factor between 0.0 and 1.0

void main()
{
    vec2 frameUV_A = vec2(
        uvA.x + frag_uv.x * (uvA.z - uvA.x),
        uvA.y + frag_uv.y * (uvA.w - uvA.y)
    );

    frameUV_A = vec2(
        uvA.x + frag_uv.x * (uvA.z - uvA.x),
        uvA.y + frag_uv.y * (uvA.w - uvA.y)
    );

    vec2 frameUV_B = vec2(
        uvB.x + frag_uv.x * (uvB.z - uvB.x),
        uvB.y + frag_uv.y * (uvB.w - uvB.y)
    );

    vec4 colA = texture(light_texture, frameUV_A);
    vec4 colB = texture(light_texture, frameUV_B);

    // linear interpolate
    outColor = mix(colA, colB, t);
}