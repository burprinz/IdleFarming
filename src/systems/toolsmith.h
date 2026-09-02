#pragma once
#include "ui.h"
#include "window.h"
#include "core/registry.h"


//
// Created by luisb on 02.09.2026.
//

class ToolSystem {
public:
    static ToolSystem init(Window *window, Registry *registry, UISystem* ui) noexcept;

    void deinit() noexcept;
    void reset() noexcept;
    bool left_click() noexcept;

    void step(const float delta) noexcept;

private:
    Registry *m_registry;
    Window *m_window;
    UISystem *m_ui;

    void initButtons() noexcept;

};