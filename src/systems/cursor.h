#pragma once
#include "ui.h"
#include "window.h"
#include "core/registry.h"

//
// Created by luisb on 19.09.2026.
//

class CursorSystem {
public:
    static CursorSystem init(Window *window, Registry *registry, UISystem* ui) noexcept;

    void deinit() noexcept;
    void reset() noexcept;

    void step(const float delta) noexcept;

private:
    Registry *m_registry;
    Window *m_window;
    UISystem *m_ui;

    bool normal_cursor_active = true;
    int field_tool_cursor_type = PLANTING;
    std::string current_tool_name = "";

};

