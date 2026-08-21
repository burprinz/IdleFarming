#pragma once
//
// Created by luisb on 21.08.2026.
//

#include "window.h"
#include "core/registry.h"

class AchievementSystem {
    public:
    AchievementSystem() = default;
    ~AchievementSystem() = default;

    static AchievementSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;

private:

    Registry* m_registry;
    Window* m_window;
};
