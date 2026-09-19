#pragma once
#include "ui.h"
#include "window.h"
#include "core/load.h"
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

    Texture axe, basket, crane, hoe, sickle, truck, wrecking_ball;
    inline void init_cursor_texture(Texture &text, std::string name) {
        std::string path = "cursor/" + name + ".png";
        text = create_single_texture(path);
    }


    bool normal_cursor_active = true;
    int field_tool_cursor_type = PLANTING;
    std::string current_tool_name = "";


    std::unordered_map<GLuint, GLFWcursor*> m_cursor_cache;
    GLFWcursor* m_active_cursor = nullptr;
    void switch_cursor(const Texture &text) noexcept;
    void reset_cursor() noexcept;
};

