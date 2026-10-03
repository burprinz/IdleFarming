#pragma once

//
// Created by luisb on 03.10.2026.
//

#include <vector>
#include <cmath>
#include <algorithm>
#include <utils/mesh.h>
#include <utils/shader.h>

static float hash2(int x, int y, int seed) {
    unsigned h = x * 374761393u + y * 668265263u + seed * 1442695041u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return ((h ^ (h >> 16)) & 0xFFFF) / 65535.f;
}

static float smooth(float t) { return t * t * (3.f - 2.f * t); }

static float smoothstep(float a, float b, float x) {
    float t = std::clamp((x - a) / (b - a), 0.f, 1.f);
    return smooth(t);
}

static float valueNoise(float x, float y, int seed) {
    int xi = (int)std::floor(x), yi = (int)std::floor(y);
    float tx = smooth(x - xi), ty = smooth(y - yi);
    float a = hash2(xi,     yi,     seed);
    float b = hash2(xi + 1, yi,     seed);
    float c = hash2(xi,     yi + 1, seed);
    float d = hash2(xi + 1, yi + 1, seed);
    float top = a + (b - a) * tx;
    float bot = c + (d - c) * tx;
    return top + (bot - top) * ty;
}

static float fbm(float x, float y, int seed) {
    float sum = 0.f, amp = 0.5f;
    for (int i = 0; i < 5; ++i) {
        sum += valueNoise(x, y, seed + i) * amp;
        x *= 2.f; y *= 2.f; amp *= 0.5f;
    }
    return sum;
}

GLuint makeCloudTexture(int w, int h, int seed) {
    std::vector<unsigned char> px(w * h * 4);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // Ellipse: Mitte = 1, Rand = 0
            float nx = (x / (float)w) * 2.f - 1.f;
            float ny = (y / (float)h) * 2.f - 1.f;
            float falloff = std::clamp(1.f - std::sqrt(nx * nx + ny * ny * 1.8f), 0.f, 1.f);

            float n       = fbm(x * 0.025f, y * 0.04f, seed);
            float density = n * falloff * 2.0f;
            float alpha   = smoothstep(0.30f, 0.60f, density);

            // Unterseite leicht grau für Volumen
            float shade = 1.f - 0.22f * (y / (float)h);
            unsigned char c = (unsigned char)(255.f * shade);

            int i = (y * w + x) * 4;
            px[i + 0] = c;
            px[i + 1] = c;
            px[i + 2] = c;
            px[i + 3] = (unsigned char)(alpha * 255.f);
        }
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, px.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}



inline void initClouds(std::vector<GLuint> &cloudTextures, int am) {
    for (int i = 0; i < am; ++i)
        cloudTextures.push_back(makeCloudTexture(256, 128, 1000 + i * 77));
}

inline GLuint randomCloudTexture(std::vector<GLuint> &cloudTextures) {
    return cloudTextures[rand() % cloudTextures.size()];
}