//
// Created by luisb on 23.06.2026.
//

#include "field.h"

#include <iostream>

#include "../helpers/util.h"
#include "core/multiplier.h"
#include "helpers/field_editor.h"
#include "helpers/sizes.h"

FieldSystem FieldSystem::init(Window *window, Registry *registry, UISystem* ui) noexcept {
    FieldSystem self;
    self.m_registry = registry;
    self.m_window = window;
    self.m_ui = ui;
    return self;
}

void FieldSystem::deinit() noexcept {

}

void FieldSystem::reset() noexcept {
    initButtons();
}

bool FieldSystem::left_click() noexcept {
    if (m_ui->check_text_button_list(m_ui->m_field_mode_buttons)) {
        return true;
    }
    if (m_ui->check_text_button_list(m_ui->m_field_plant_select_buttons[m_registry->current_selected_field])) {
        return true;
    }
    if (m_registry->field_mode == FIELD_FARMING) {
        // Planting / Harvesting
        if (m_ui->check_text_button_list(m_ui->m_field_helper_buttons[m_registry->current_selected_field])) {
            return true;
        }
    } else if (m_registry->field_mode == FIELD_HELPER_CONFIGURATION) {
        // Helper Config
        if (m_ui->check_text_button_list(m_ui->m_field_helper_config_buttons)) {
            return true;
        }
    }
    return false;
}


void FieldSystem::initButtons() noexcept {
    m_ui->m_field_plant_select_buttons.clear();
    m_ui->m_field_mode_buttons.clear();
    m_ui->m_field_helper_buttons.clear();
    m_ui->m_field_helper_config_buttons.clear();

    Registry* registry = m_registry;

    glm::vec2 top_left = field_cell_topleft;
    glm::vec2 select_button_size = normal_button_size;
    glm::vec2 center = calc_center(top_left, select_button_size);


    /**
     * Field selection
     */
    for (size_t field_id = 0; field_id < m_registry->field_order.size(); field_id++) {
        entt::entity field_entity = m_registry->field_order[field_id];
        Field f = m_registry->ecs.get<Field>(field_entity);

        // Field Switch
        TextButton fieldB;
        fieldB.text = f.display_name;
        fieldB.lang_label = f.display_name;
        fieldB.pos = center;
        fieldB.size = select_button_size;
        fieldB.id_label = field_entity;
        fieldB.on_click = [registry, field_id, field_entity] {
            Unlockable u = registry->ecs.get<Unlockable>(field_entity);
            if (!u.unlocked) {
                return;
            }

            if (registry->field_mode == FIELD_FARMING) {
                registry->current_selected_field = field_id;
            } else if (registry->field_mode == FIELD_HELPER_CONFIGURATION) {
                if (registry->ecs.get<Unlockable>(registry->field_helper[field_entity]).unlocked) {
                    registry->current_selected_field = field_id;
                }
            }
        };
        m_ui->m_field_mode_buttons.push_back(fieldB);

        // Plant Selection
        std::vector<TextButton> plant_select_buttons;
        initPlantLikePlantSelectionButtons(m_registry->plant_order[field_id], m_registry->last_selected_plant[field_id], m_registry->current_selected_plant[field_id], plant_select_buttons);
        m_ui->m_field_plant_select_buttons.push_back(plant_select_buttons);

        // Helper Configuration
        std::vector<TextButton> helper_buttons;
        entt::entity helper_id = m_registry->field_helper[field_entity];
        TextButton helper_config_button;
        helper_config_button.text = helper_id!=entt::null?m_registry->ecs.get<FarmHelper>(helper_id).name:"No Helper";
        helper_config_button.pos = calc_center(field_helper_topleft, medium_button_size);
        helper_config_button.size = medium_button_size;
        helper_config_button.id_label = helper_id;
        helper_config_button.on_click = [helper_id, registry] {
            if (helper_id == entt::null) {
                return;
            }

            Unlockable u = registry->ecs.get<Unlockable>(helper_id);
            if (!u.unlocked) {
                return;
            }

            registry->field_mode = FIELD_HELPER_CONFIGURATION;
        };
        helper_buttons.push_back(helper_config_button);
        m_ui->m_field_helper_buttons.push_back(helper_buttons);

        center.x += select_button_size.x + normal_gap;
    }

    TextButton helper_back_button;
    helper_back_button.text = "Back";
    helper_back_button.pos = helper_config_topleft - glm::vec2{0, helper_config_size.y} + glm::vec2{normal_gap, normal_gap} + normal_button_size / 2.0f;
    helper_back_button.size = normal_button_size;
    helper_back_button.unselect_color = white;
    helper_back_button.on_click = [registry] {
        registry->field_mode = FIELD_FARMING;
    };
    m_ui->m_field_helper_config_buttons.push_back(helper_back_button);
}

void FieldSystem::initPlantLikePlantSelectionButtons(std::vector<entt::entity> order, entt::entity &last_selected, entt::entity &current_selected, std::vector<TextButton> &buttons) noexcept {
    buttons.clear();
    glm::vec2 top_left = field_plant_selection_topleft;
    glm::vec2 size = medium_button_size;
    glm::vec2 center = calc_center(top_left, size);
    for (entt::entity id : order) {
        TextButton plantB;
        plantB.pos = center;
        plantB.size = size;
        plantB.id_label = id;
        plantB.on_click = [&last_selected, &current_selected, id] {
            last_selected = current_selected;
            current_selected = id;
        };
        buttons.push_back(plantB);

        center.y -= size.y + normal_gap;
    }
}



void FieldSystem::step(const float delta) noexcept {
    if (m_registry->window() == FIELDS && m_registry->field_mode == FIELD_FARMING) {
        try_to_plant(
            m_registry->plant_field_tiles[m_registry->current_selected_field],
            m_registry->current_selected_plant[m_registry->current_selected_field]
            );
    }

    // Update every cell if not empty
    for (entt::entity fID : m_registry->ecs.view<FarmField>()) {
        FarmField &f = m_registry->ecs.get<FarmField>(fID);
        Field field = m_registry->ecs.get<Field>(f.parent_field);
        float growspeed = delta * Multiplier::instance().get_stat(field.growth_multiplier);
        if (!f.is_empty && !f.harvestable && !f.helper_active) {
            apply_growtime(m_registry, fID, growspeed);
        }
    }

}

void FieldSystem::try_to_plant(std::vector<std::vector<entt::entity>> field, entt::entity p_id) noexcept {

    Field current_field = m_registry->ecs.get<Field>(m_registry->field_order[m_registry->current_selected_field]);

    glm::vec2 mousePos = m_window->getMousePosition();
    bool pointFound = false;

    int plant_radius = 0;
    int harvest_radius = 0;
    int collect_radius = 0;

    if (!m_registry->keys["strg"]) {
        entt::entity plant_tool_id = m_registry->current_plant_tools[current_field.type];
        if (plant_tool_id != entt::null) {
            plant_radius = m_registry->ecs.get<Tool>(plant_tool_id).radius;
        }

        entt::entity harvest_tool_id = m_registry->current_harvest_tools[current_field.type];
        if (harvest_tool_id != entt::null) {
            harvest_radius = m_registry->ecs.get<Tool>(harvest_tool_id).radius;
        }

        entt::entity collect_tool_id = m_registry->current_collect_tools[current_field.type];
        if (collect_tool_id != entt::null) {
            collect_radius = m_registry->ecs.get<Tool>(collect_tool_id).radius;
        }
    }

    for (size_t y = 0; y < field.size(); y++) {
        std::vector<entt::entity> row = field[y];
        for (size_t x = 0; x < row.size(); x++) {
            entt::entity fieldID = row[x];

            FarmField &f = m_registry->ecs.get<FarmField>(fieldID);
            glm::vec2 pos = m_registry->ecs.get<Position>(fieldID).pos;
            glm::vec2 size = m_registry->ecs.get<Dimension>(fieldID).dim;

            // Check if mouse is over cell if no cell already found and plant is selected
            if (!f.helper_active && !pointFound && point_in_rect(mousePos, pos, size)) {
                pointFound = true;
                f.marked = true;


                // Left Click and Plant selected -> Try to plant in radius
                if (m_registry->keys["mouse_left"] && p_id != entt::null) {
                    plant_with_radius(m_registry, field, p_id, x, y, plant_radius);
                }

                // Right CLick -> Try to harvest in radius
                if (m_registry->keys["mouse_right"]) {

                    if (current_field.plant_type == NORMAL_PLANT || m_registry->keys["shift"]) {
                        // Normale Pflanze oder regrow/inv + shift -> Ernten und entfernen
                        harvest_with_radius(m_registry, field, x, y, harvest_radius);
                    } else {
                        // regrow/inv -> Collecten
                        collect_with_radius(m_registry, field, x, y, collect_radius);
                    }

                }

            } else {
                // If cell already found or mouse does not hover over cell
                f.marked = false;
            }


        }
    }
}




