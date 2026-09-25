#pragma once

#include "ui.h"
#include "window.h"
#include "core/registry.h"
//
// Created by luisb on 25.09.2026.
//




class SettingsSystem {
public:
    static SettingsSystem init(Window *window, Registry *registry, UISystem* ui) noexcept;

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