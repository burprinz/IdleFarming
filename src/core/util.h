#pragma once

#include <common.h>

#include "config.h"
#include "registry.h"
#include "window.h"

// Template Functions for often reused Function calls searched for and created with help of ChatGPT

//Non-owning pointer guard
//Helps counter current issues with Pointers no longer existing.
template<typename T>
T& must(T* p, const char* msg) {
    assert(p && msg);
    return *p;
}

//Rect hit-test helper
inline bool point_in_rect(glm::vec2 p, glm::vec2 center, glm::vec2 size) {
    return center.x - size.x/2 <= p.x && p.x <= center.x + size.x/2
        && center.y - size.y/2 <= p.y && p.y <= center.y + size.y/2;
}

//Distance helper
inline float dist(glm::vec2 a, glm::vec2 b) {
    glm::vec2 d = a - b;
    return std::sqrt(d.x*d.x + d.y*d.y);
}

inline bool rectCollision(const Rect& r1, const Rect& r2) {
    return r1.pos.x < r2.pos.x + r2.size.x && r1.pos.x + r1.size.x > r2.pos.x && r1.pos.y < r2.pos.y + r2.size.y && r1.pos.y + r1.size.y > r2.pos.y;
}

inline bool isFullyAboveRect(const glm::vec2 pos, const Rect& belowRect) {
    return pos.y > belowRect.pos.y + belowRect.size.y;
}

inline PolygonShape createConvexPolygon(std::vector<glm::vec2> points) {
    PolygonShape shape;
    shape.vertices = points;

    return shape;
}