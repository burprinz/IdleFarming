//
// Created by luisb on 04.08.2026.
//

#include "helper.h"

#include <iostream>

#include "helpers/finder.h"
#include "helpers/sizes.h"
#include "helpers/util.h"


HelperSystem HelperSystem::init(Window *window, Registry *registry) noexcept {
    HelperSystem self;
    self.m_registry = registry;
    self.m_window = window;
    return self;
}

void HelperSystem::deinit() noexcept {

}

void HelperSystem::reset() noexcept {

}

void HelperSystem::step(const float delta) noexcept {
    (void) delta;

    if (m_registry->field_mode == FIELD_HELPER_CONFIGURATION) {
        glm::vec2 mousePos = m_window->getMousePosition();

        size_t field_id = m_registry->current_selected_field;
        entt::entity field_entity = m_registry->field_order[field_id];
        entt::entity helper_id = m_registry->field_helper[field_entity];
        entt::entity plant_id = m_registry->current_selected_plant[field_id];
        std::vector<std::vector<entt::entity>> field = m_registry->plant_field_tiles[field_id];

        FarmHelper &helper = m_registry->ecs.get<FarmHelper>(helper_id);

        bool point_found = false;

        // Try to add or remove managing field
        for (size_t y = 0; y < field.size(); y++) {
            std::vector<entt::entity> row = field[y];
            for (size_t x = 0; x < row.size(); x++) {
                entt::entity fieldID = row[x];
                FarmField &f = m_registry->ecs.get<FarmField>(fieldID);
                glm::vec2 pos = m_registry->ecs.get<Position>(fieldID).pos + helper_config_field_offset;
                glm::vec2 size = m_registry->ecs.get<Dimension>(fieldID).dim;

                bool mouse_in_rect = point_in_rect(mousePos, pos, size);

                // Marking
                if (!point_found && mouse_in_rect) {
                    point_found = true;
                    f.marked = true;
                } else {
                    f.marked = false;
                }

                // Left Click -> Try to add new
                if (mouse_in_rect
                    && m_registry->keys["mouse_left"]
                    && helper.working_area.size() < static_cast<size_t>(helper.amount_fields)
                    && plant_id != entt::null
                    && !is_in_list(helper.working_area, fieldID)) {
                        f.helper_active = true;
                        helper.working_area.push_back(fieldID);
                        helper.working_plants.push_back(plant_id);
                }

                // Right Click -> Try to remove
                if (mouse_in_rect && m_registry->keys["mouse_right"] && helper.working_area.size() > 0) {
                    for (size_t i = 0; i < helper.working_area.size(); i++) {
                        if (helper.working_area[i] == fieldID) {
                            // Field is in working array -> Remove
                            f.helper_active = false;
                            helper.working_area.erase(helper.working_area.begin()+i);
                            helper.working_plants.erase(helper.working_plants.begin()+i);
                            break;
                        }
                    }
                }
            }
        }
    }



    // Check every Helper Field
    for (entt::entity h_id : m_registry->ecs.view<FarmHelper>()) {
        FarmHelper helper = m_registry->ecs.get<FarmHelper>(h_id);
        Field f = m_registry->ecs.get<Field>(helper.field);

        for (size_t i = 0; i < helper.working_area.size(); i++) {
            entt::entity field_id = helper.working_area[i];
            FarmField &field = m_registry->ecs.get<FarmField>(field_id);
            entt::entity plant_id = helper.working_plants[i];
            Plant &plant = m_registry->ecs.get<Plant>(plant_id);

            // HARVESTING
            if (f.plant_type == NORMAL_PLANT) {
                // Normale Pflanze
                if (!field.is_empty && field.harvestable) {
                    BigNumber &am = m_registry->ecs.get<Amount>(plant_id).am;
                    am += plant.min_collect;
                    field.harvestable = false;
                    field.is_empty = true;
                }
            } else {
                // regrow/inv -> Collecten
                BigNumber &am = m_registry->ecs.get<Amount>(plant_id).am;
                // Try to collect
                if (plant.has_inventory && m_registry->ecs.all_of<Inventory>(field_id)) {
                    // Machine -> Collect inventory and keep everything
                    Inventory &inv = m_registry->ecs.get<Inventory>(field_id);
                    if (inv.amount == inv.size) {
                        am += inv.amount;
                        inv.amount = BigNumber(0);
                    }
                } else if (field.harvestable) {
                    // No Machine -> Collect and reset growtime
                    am += plant.min_collect;
                    field.harvestable = false;
                    field.grow = plant.growTime;
                }
            }



            // PLANTING
            if (field.is_empty) {
                // Plant if field is empty and left click pressed
                field.is_empty = false;
                field.plant = plant_id;
                field.grow = 0.0;
                field.harvestable = false;
                Plant p = m_registry->ecs.get<Plant>(plant_id);
                if (p.has_inventory) {
                    Inventory inv;
                    inv.size = BigNumber(p.inventory_size);
                    inv.amount = BigNumber(0);
                    m_registry->ecs.emplace<Inventory>(field_id, inv);
                }
            }
        }

    }

}

// TODO: Use Same field with helper offset for marked.