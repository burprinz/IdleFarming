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


    void blackjackStep(const float delta);

};
