#pragma once
#include "window.h"
#include "core/registry.h"
//
// Created by luisb on 27.08.2026.
//

class GambleSystem {
    public:
    static GambleSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;

private:

    Registry* m_registry;
    Window* m_window;

    bool animation_done = false;
    bool animation_decelerating = false;
    float animation_pos = 0.0f;
    float animation_vel = 0.0f;
    float animation_acc = 33.5f; // 1s = 4.45
    float animation_min_vel = 0.4f; // 1s = 0.3

    void blackjackStep(const float delta);

};
