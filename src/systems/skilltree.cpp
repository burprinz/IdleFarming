//
// Created by luisb on 01.09.2026.
//

#include "skilltree.h"

#include "helpers/sizes.h"

SkilltreeSystem SkilltreeSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    SkilltreeSystem self;
    self.m_registry = registry;
    self.m_window = window;
    self.m_ui = ui;
    return self;
}

void SkilltreeSystem::deinit() noexcept {

}

void SkilltreeSystem::reset() noexcept {
    init_buttons();
}

void SkilltreeSystem::init_buttons() noexcept {
    m_ui->m_skilltree_buttons.clear();

    TextButton unlock_plants_field;
    unlock_plants_field.text = "Unlock Plants Field";
    unlock_plants_field.pos = {config::CAMERA_VIEW_WIDTH/2.0f, config::CAMERA_VIEW_HEIGHT/2.0f};
    unlock_plants_field.size = skill_button_size;
    unlock_plants_field.on_click = [] {

    };
    m_ui->m_skilltree_buttons.push_back(unlock_plants_field);
}


bool SkilltreeSystem::left_click() noexcept {
    return m_ui->check_text_button_list(m_ui->m_skilltree_buttons);
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

