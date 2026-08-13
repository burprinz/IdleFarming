//
// Created by luisb on 23.06.2026.
//

#include "field.h"

#include <iostream>

#include "../helpers/util.h"
#include "helpers/field_editor.h"

FieldSystem FieldSystem::init(Window *window, Registry *registry) noexcept {
    FieldSystem self;
    self.m_registry = registry;
    self.m_window = window;
    return self;
}

void FieldSystem::deinit() noexcept {

}

void FieldSystem::reset() noexcept {

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
        if (!f.is_empty && !f.harvestable && !f.helper_active) {
            apply_growtime(m_registry, fID, delta);
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




