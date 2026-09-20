//
// Created by luisb on 02.09.2026.
//

#include "village.h"

#include "helpers/sizes.h"

VillageSystem VillageSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    VillageSystem self;
    self.m_window = window;
    self.m_registry = registry;
    self.m_ui = ui;
    return self;
}

void VillageSystem::deinit() noexcept {

}

void VillageSystem::reset() noexcept {
    initButtons();
}

void VillageSystem::initButtons() noexcept {

}

bool VillageSystem::left_click() noexcept {
    //return m_ui->check_text_button_list(m_ui->m_toolsmith_buy_buttons);
    return false;
}

void VillageSystem::step(const float delta) noexcept {
    (void) delta;

    if (m_registry->window() == VILLAGE) {
        glm::vec2 mouse_pos = m_window->getMousePosition();

        if (m_registry->keys["mouse_right"]) {
            if (!right_click_pressed && point_in_rect(mouse_pos, village_window_center, village_window_size)) {
                right_click_pressed = true;
                origin_mouse = mouse_pos;
                old_offset = m_registry->village_pos_offset;
            }
            if (right_click_pressed) {
                glm::vec2 next_offset = mouse_pos - origin_mouse;
                m_registry->village_pos_offset = old_offset + next_offset;
            }
        } else {
            right_click_pressed = false;
        }
    }
}




