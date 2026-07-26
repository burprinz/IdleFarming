#include "camera.h"

#include <algorithm>
#include <iostream>

#include "../helpers/config.h"

CameraSystem CameraSystem::init(Registry* registry) noexcept {
    CameraSystem self;

    self.m_registry = registry;

    auto& cam = self.m_registry->m_camera;
    cam.viewSize = { config::CAMERA_VIEW_WIDTH, config::CAMERA_VIEW_HEIGHT };

    //Boundries to the Edges of the Screen.
    cam.bufferLeft = 0.35f;
    cam.bufferRight = 0.35f;
    cam.bufferTop = 0.40f;
    cam.bufferBottom = 0.30f;

    //Starts Camera in the center of the Screen since Player will not be created yet.
    cam.center = 0.5f * cam.viewSize;
    self.m_registry->camera_origin = cam.center;

    // PLACEHOLDER, this should be either calculated by objects or read in through
    cam.worldMin = { 0.f, 0.f };
    cam.worldMax = { 1000.f, 1000.f }; // intentionally large placeholder

    return self;
}

void CameraSystem::step(const float delta) {
    (void) delta;
    /*
    CameraState& cam = m_registry->m_camera;

    glm::vec2 player_pos = m_registry->ecs.get<Position>(m_registry->player()).pos;
    glm::vec2 half = 0.5f * cam.viewSize;

    //Visable Rectangle in World Coordinates.
    float left   = cam.center.x - half.x;
    float right  = cam.center.x + half.x;
    //float bottom = cam.center.y - half.y;
    //float top    = cam.center.y + half.y;

    // Concept of deadzone for improved Camera Movement. https://medium.com/codex/make-your-godot-2d-camera-follow-your-player-66395e7af6ef
    float dzLeft   = left   + cam.bufferLeft   * cam.viewSize.x;
    float dzRight  = right  - cam.bufferRight  * cam.viewSize.x;
    //float dzBottom = bottom + cam.bufferBottom * cam.viewSize.y;
    //float dzTop    = top    - cam.bufferTop    * cam.viewSize.y;

    glm::vec2 targetCenter = cam.center;

    if (player_pos.x < dzLeft)  targetCenter.x -= (dzLeft - player_pos.x);
    if (player_pos.x > dzRight) targetCenter.x += (player_pos.x - dzRight);
    //if (player_pos.y < dzBottom) targetCenter.y -= (dzBottom - player_pos.y);
    //if (player_pos.y > dzTop)    targetCenter.y += (player_pos.y - dzTop);

    // Ensuring View stays in World
    float minCx = cam.worldMin.x + half.x;
    float maxCx = cam.worldMax.x - half.x;
    float minCy = cam.worldMin.y + half.y;
    float maxCy = cam.worldMax.y - half.y;

    targetCenter.x = std::clamp(targetCenter.x, minCx, maxCx);
    targetCenter.y = std::clamp(targetCenter.y, minCy, maxCy);

	float alpha = 1.0f - std::exp(-CAMERA_SMOOTHING_FACTOR * delta);
	cam.center = glm::mix(cam.center, targetCenter, alpha);

    m_registry->camera_pos = cam.center;
    */
}

void CameraSystem::reset() noexcept {}