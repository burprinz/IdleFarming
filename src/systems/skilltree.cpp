//
// Created by luisb on 01.09.2026.
//

#include "skilltree.h"

#include "core/multiplier.h"
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

    Registry *registry = m_registry;
    (void) registry;

    glm::vec2 center = {config::CAMERA_VIEW_WIDTH/2.0f, config::CAMERA_VIEW_HEIGHT/2.0f};

    entt::entity flower_entity = m_registry->field_order[m_registry->field_ids["flowers"]];
    Unlockable &field_unlock = m_registry->ecs.get<Unlockable>(flower_entity);
    entt::entity unlock_flower_field_id = create_skill(center,
                "Flowers",
                "Unlocks Flower Field",
                [&field_unlock] {
                    field_unlock.unlocked = true;
                }, 1);

    entt::entity tree_entity = m_registry->field_order[m_registry->field_ids["trees"]];
    Unlockable &tree_unlock = m_registry->ecs.get<Unlockable>(tree_entity);
    entt::entity unlock_tree_field_id = create_skill(center + glm::vec2{3, 0},
                "Trees",
                "Unlocks Tree Field",
                [&tree_unlock] {
                    tree_unlock.unlocked = true;
                }, 1, unlock_flower_field_id);


    entt::entity cave_entity = m_registry->field_order[m_registry->field_ids["cave_machines"]];
    Unlockable &cave_unlock = m_registry->ecs.get<Unlockable>(cave_entity);
    entt::entity unlock_cave_field_id = create_skill(center + glm::vec2{6, 2},
                "Cave",
                "Unlocks Cave Machines",
                [&cave_unlock] {
                    cave_unlock.unlocked = true;
                }, 1, unlock_tree_field_id);

    entt::entity grow_speed_id = create_skill(center + glm::vec2{-3, 0},
                "Grow Speed",
                "Grow Speed multiplier + 1",
                [] {
                    Multiplier::instance().higher("skilltree_overall_growth", 1000);
                }, 10, unlock_flower_field_id);

    (void) unlock_cave_field_id;
    (void) grow_speed_id;
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

