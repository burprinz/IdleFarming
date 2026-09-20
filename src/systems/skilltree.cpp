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

    Registry *registry = m_registry;
    (void) registry;

    glm::vec2 center = {config::CAMERA_VIEW_WIDTH/2.0f, config::CAMERA_VIEW_HEIGHT/2.0f};

    entt::entity flower_entity = m_registry->field_order[m_registry->field_ids["flowers"]];
    Unlockable &field_unlock = m_registry->ecs.get<Unlockable>(flower_entity);
    entt::entity unlock_flower_field_id = entt::null;
    unlock_flower_field_id = create_skill(center,
                "unlock_flower_field",
                "Flowers",
                "Unlocks Flower Field",
                {},
                [&field_unlock](entt::entity id) {
                    (void) id;
                    field_unlock.unlocked = true;
                }, 1);
    m_registry->skilltree_entities["unlock_flower_field"] = unlock_flower_field_id;

    entt::entity flower_speed_01_id = create_skill(center + glm::vec2{0, 2},
                "flower_speed_01",
                "Flowers Grow Speed",
                "Flowers Grow Speed multiplier + 0.2",
                {"skilltree_flowers_growth"},
                [registry](entt::entity id) {
                    Skill s = registry->ecs.get<Skill>(id);
                    Multiplier::instance().set("skilltree_flowers_growth", 1+s.level*0.2);
                }, 10, unlock_flower_field_id);
    m_registry->skilltree_entities["flower_speed_01"] = flower_speed_01_id;

    entt::entity tree_entity = m_registry->field_order[m_registry->field_ids["trees"]];
    Unlockable &tree_unlock = m_registry->ecs.get<Unlockable>(tree_entity);
    entt::entity unlock_tree_field_id = create_skill(center + glm::vec2{3, 0},
                "unlock_tree_field",
                "Trees",
                "Unlocks Tree Field",
    {},
                [&tree_unlock](entt::entity id) {
                    (void)id;
                    tree_unlock.unlocked = true;
                }, 1, unlock_flower_field_id);
    m_registry->skilltree_entities["unlock_tree_field"] = unlock_tree_field_id;


    entt::entity tree_speed_01_id = create_skill(center + glm::vec2{3, 2},
                "tree_speed_01",
            "Trees Grow Speed",
            "Trees Grow Speed multiplier + 0.2",
                {"skilltree_trees_growth"},
            [registry](entt::entity id) {
                Skill s = registry->ecs.get<Skill>(id);
                Multiplier::instance().set("skilltree_trees_growth", 1+s.level*0.2);
            }, 10, unlock_tree_field_id);
    m_registry->skilltree_entities["tree_speed_01"] = tree_speed_01_id;


    entt::entity cave_entity = m_registry->field_order[m_registry->field_ids["cave_machines"]];
    Unlockable &cave_unlock = m_registry->ecs.get<Unlockable>(cave_entity);
    entt::entity unlock_cave_field_id = create_skill(center + glm::vec2{6, 0},
                "unlock_cave_field",
                "Cave",
                "Unlocks Cave Machines",
            {},
                [&cave_unlock](entt::entity id) {
                    (void)id;
                    cave_unlock.unlocked = true;
                }, 1, unlock_tree_field_id);
    m_registry->skilltree_entities["unlock_cave_field"] = unlock_cave_field_id;


    entt::entity cave_speed_01_id = create_skill(center + glm::vec2{6, 2},
                "cave_speed_01",
            "Caves Grow Speed",
            "Caves Grow Speed multiplier + 0.2",
{"skilltree_cave_machines_growth"},
            [registry](entt::entity id) {
                Skill s = registry->ecs.get<Skill>(id);
                Multiplier::instance().set("skilltree_cave_machines_growth", 1+s.level*0.2);
            }, 10, unlock_cave_field_id);
    m_registry->skilltree_entities["cave_speed_01"] = cave_speed_01_id;


    entt::entity grow_speed_01_id = create_skill(center + glm::vec2{-3, 0},
                "grow_speed_01",
                "Grow Speed",
                "Grow Speed multiplier + 0.1",
        {"skilltree_overall_growth"},
                [registry](entt::entity id) {
                    Skill s = registry->ecs.get<Skill>(id);
                    Multiplier::instance().set("skilltree_overall_growth", 1+s.level*0.2);
                }, 10);
    m_registry->skilltree_entities["grow_speed_01"] = grow_speed_01_id;


    entt::entity plants_speed_01_id = create_skill(center + glm::vec2{-3, 2},
                "plants_speed_01",
                "Plants Grow Speed",
                "Plants Grow Speed multiplier + 0.2",
                {"skilltree_plants_growth"},
                [registry](entt::entity id) {
                    Skill s = registry->ecs.get<Skill>(id);
                    Multiplier::instance().set("skilltree_plants_growth", 1+s.level*0.2);
                }, 10, grow_speed_01_id);
    m_registry->skilltree_entities["plants_speed_01"] = plants_speed_01_id;

}


bool SkilltreeSystem::left_click() noexcept {
    return m_ui->check_text_button_list(m_ui->m_skilltree_buttons);
}


void SkilltreeSystem::step(const float delta) noexcept {
    (void) delta;

    if (m_registry->window() == SKILLTREE) {
        glm::vec2 mouse_pos = m_window->getMousePosition();

        if (m_registry->keys["mouse_right"]) {
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

        if (m_registry->keys["vertical_scroll_up"]) {
            glm::vec2 anchor = mouse_pos;
            float old_zoom = m_registry->skilltree_zoom;
            m_registry->skilltree_zoom += delta;
            if (m_registry->skilltree_zoom > 1) {
                m_registry->skilltree_zoom = 1;
            }
            float new_zoom = m_registry->skilltree_zoom;
            glm::vec2 world_point = (anchor - m_registry->skilltree_pos_offset) / old_zoom;
            m_registry->skilltree_pos_offset = anchor - world_point * new_zoom;
        } else if (m_registry->keys["vertical_scroll_down"]) {
            glm::vec2 anchor = mouse_pos;
            float old_zoom = m_registry->skilltree_zoom;
            m_registry->skilltree_zoom -= delta;
            if (m_registry->skilltree_zoom < 0.1) {
                m_registry->skilltree_zoom = 0.1;
            }
            float new_zoom = m_registry->skilltree_zoom;
            glm::vec2 world_point = (anchor - m_registry->skilltree_pos_offset) / old_zoom;
            m_registry->skilltree_pos_offset = anchor - world_point * new_zoom;
        }
    }

}

