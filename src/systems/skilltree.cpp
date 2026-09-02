//
// Created by luisb on 01.09.2026.
//

#include "skilltree.h"

SkilltreeSystem SkilltreeSystem::init(Window *window, Registry *registry) noexcept {
    SkilltreeSystem self;
    self.m_registry = registry;
    self.m_window = window;
    return self;
}

void SkilltreeSystem::deinit() noexcept {

}

void SkilltreeSystem::reset() noexcept {

}

void SkilltreeSystem::step(const float delta) noexcept {
    (void) delta;

    if (m_registry->window() == SKILLTREE) {
        if (m_registry->keys["mouse_right"]) {
            glm::vec2 mouse_pos = m_window->getMousePosition();
            if (!right_click_pressed) {
                right_click_pressed = true;
                origin_mouse = mouse_pos;
                old_offset = m_registry->skilltree_pos_offset;
            }
            glm::vec2 next_offset = mouse_pos - origin_mouse;
            m_registry->skilltree_pos_offset = old_offset + next_offset;
        } else {
            right_click_pressed = false;
        }
    }

}

