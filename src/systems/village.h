#pragma once
#include "ui.h"
#include "window.h"
#include "core/registry.h"

//
// Created by luisb on 18.09.2026.
//

class VillageSystem {
public:
    static VillageSystem init(Window *window, Registry *registry, UISystem* ui) noexcept;

    void deinit() noexcept;
    void reset() noexcept;
    bool left_click() noexcept;

    void step(const float delta) noexcept;

private:
    Registry *m_registry;
    Window *m_window;
    UISystem *m_ui;

    void initButtons() noexcept;


    // Movement
    bool right_click_pressed = false;
    glm::vec2 origin_mouse = {0.0, 0.0};
    glm::vec2 old_offset = {0.0, 0.0};

};