//
// Created by luisb on 09.03.2026.
//

#include "config.h"
#include "core/registry.h"

inline std::vector<int> triangulatePrism(const std::vector<glm::vec2>& vertices) noexcept {
    const int n = vertices.size();
    std::vector<int> active_vertices;
    for (int i = 0; i < n; i++) {       // this vector acts as a copy of the vertices, the active vertices will shrink
        active_vertices.push_back(i);
    }
    std::vector<int> triangle_indices;

    if (n < 3) return triangle_indices; // Not enough vertices to form a triangle

    int i = 1;
    while (active_vertices.size() > 3) {
        // loop over all vertices to find ears
        int active_vertices_size = active_vertices.size();
        glm::vec2 prev = vertices[active_vertices[(i - 1 + active_vertices_size) % active_vertices_size]];
        glm::vec2 curr = vertices[active_vertices[i]];
        glm::vec2 next = vertices[active_vertices[(i + 1) % active_vertices_size]];

        float cross = (curr.x - prev.x) * (next.y - curr.y) - (curr.y - prev.y) * (next.x - curr.x);

        if (cross <= 0) {
            i++;
            continue;   // Not a convex corner, skip
        }
        int m = active_vertices.size();
        for (int j = 0; j < m; j++) {
            if (j == i - 1 || j == i || j == i + 1) continue; // Skip triangle vertices themselves
            // Cross product to check if point is inside triangle
            glm::vec2 other_point = vertices[active_vertices[j]];
            float cross1 = (curr.x - prev.x) * (other_point.y - prev.y) - (curr.y - prev.y) * (other_point.x - prev.x);
            float cross2 = (next.x - curr.x) * (other_point.y - curr.y) - (next.y - curr.y) * (other_point.x - curr.x);
            float cross3 = (prev.x - next.x) * (other_point.y - next.y) - (prev.y - next.y) * (other_point.x - next.x);
            if (!((cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0))) {
                if (j == m - 1) {
                    // No other vertex is inside the triangle, it's an ear
                    triangle_indices.push_back(active_vertices[i - 1]);
                    triangle_indices.push_back(active_vertices[i]);
                    triangle_indices.push_back(active_vertices[i + 1]);
                    active_vertices.erase(active_vertices.begin() + i);
                    i = 0;
                    break;
                }
            }
        }
        i++;
    }
    triangle_indices.push_back(active_vertices[0]);
    triangle_indices.push_back(active_vertices[1]);
    triangle_indices.push_back(active_vertices[2]);

    return triangle_indices;
}

inline BoundingBox createBoundingBox(std::vector<glm::vec2> vertices) noexcept {
    glm::vec2 min = {config::FLOAT_MAX, config::FLOAT_MAX};
    glm::vec2 max = {config::FLOAT_MIN, config::FLOAT_MIN};
    for (glm::vec2 point : vertices) {
        if (point.x < min.x) min.x = point.x;
        if (point.x > max.x) max.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.y > max.y) max.y = point.y;
    }
    return BoundingBox(min, max);
}

inline PolygonShape createPolygon(std::vector<glm::vec2> counter_clock_vertices) {
    PolygonShape shape;
    shape.vertices = counter_clock_vertices;
    shape.triangle_indices = triangulatePrism(counter_clock_vertices);
    shape.bounding_box = createBoundingBox(counter_clock_vertices);
    return shape;
}

inline Rect boundingBoxToRect(BoundingBox bbox, glm::vec2 pos) noexcept {
    Rect rect;
    rect.pos = bbox.min + pos;
    rect.size = bbox.max - bbox.min;
    return rect;
}

inline bool rectangleRectangeCollision(Rect r1, Rect r2) {
    return r1.pos.x < r2.pos.x + r2.size.x
        && r1.pos.x + r1.size.x > r2.pos.x
        && r1.pos.y < r2.pos.y + r2.size.y
        && r1.pos.y + r1.size.y > r2.pos.y;
}


inline bool rectanglePolygonCollision(Rect rect, PolygonShape poly, glm::vec2 poly_pos) {
    if (!rectangleRectangeCollision(rect, boundingBoxToRect(poly.bounding_box, poly_pos))) return false;

    // Rect inside AABB
    glm::vec2 rectPoints[4] = {
        rect.pos,
        rect.pos + glm::vec2(rect.size.x, 0),
        rect.pos + rect.size,
        rect.pos + glm::vec2(0, rect.size.y),
    };

    for (size_t i = 0; i < poly.vertices.size(); i++) {
        glm::vec2 p1 = poly.vertices[i] + poly_pos;
        glm::vec2 p2 = poly.vertices[(i + 1) % poly.vertices.size()] + poly_pos;
        glm::vec2 edge = p2 - p1;

        // Normalize
        glm::vec2 axis = glm::vec2(-edge.y, edge.x);

        float minR = config::FLOAT_MAX;
        float maxR = config::FLOAT_MIN;
        float minP = config::FLOAT_MAX;
        float maxP = config::FLOAT_MIN;

        for (glm::vec2 point : rectPoints) {
            float projection = glm::dot(point, axis);
            if (projection < minR) minR = projection;
            if (projection > maxR) maxR = projection;
        }

        for (glm::vec2 point : poly.vertices) {
            float projection = glm::dot(point + poly_pos, axis);
            if (projection < minP) minP = projection;
            if (projection > maxP) maxP = projection;
        }

        if (maxR < minP || maxP < minR) return false;
    }

    return true;
}