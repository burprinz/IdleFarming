#pragma once
#include "window.h"
#include "core/registry.h"

//
// Created by luisb on 30.06.2026.
//

class UnlockSystem {
public:
    UnlockSystem() = default;
    ~UnlockSystem() = default;

    static UnlockSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;
    void step(const float delta) noexcept;
    void reset() noexcept;

private:
    Registry* m_registry;
    Window* m_window;

    void checkPlantUnlock() noexcept;

};