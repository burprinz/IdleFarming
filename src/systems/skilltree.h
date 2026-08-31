//
// Created by luisb on 01.09.2026.
//

#include "window.h"
#include "core/registry.h"

class SkilltreeSystem {
public:
    static SkilltreeSystem init(Window *window, Registry *registry) noexcept;

    void deinit() noexcept;

    void step(const float delta) noexcept;

    void reset() noexcept;

private:
    Registry *m_registry;
    Window *m_window;

    bool right_click_pressed = false;
    glm::vec2 origin_mouse = {0.0, 0.0};
    glm::vec2 old_offset = {0.0, 0.0};

};
