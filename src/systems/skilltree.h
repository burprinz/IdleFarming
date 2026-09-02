//
// Created by luisb on 01.09.2026.
//

#include "ui.h"
#include "window.h"
#include "core/registry.h"

class SkilltreeSystem {
public:
    static SkilltreeSystem init(Window *window, Registry *registry, UISystem *ui) noexcept;

    void deinit() noexcept;

    void reset() noexcept;
    void init_buttons() noexcept;
    bool left_click() noexcept;

    void step(const float delta) noexcept;


private:
    Registry *m_registry;
    Window *m_window;
    UISystem *m_ui;

    bool right_click_pressed = false;
    glm::vec2 origin_mouse = {0.0, 0.0};
    glm::vec2 old_offset = {0.0, 0.0};

};
