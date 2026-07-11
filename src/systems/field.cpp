//
// Created by luisb on 23.06.2026.
//

#include "field.h"

#include <iostream>

#include "core/util.h"

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

    if (m_registry->window() == FIELDS) {
        try_to_plant(
            m_registry->tmp_field_tiles[m_registry->current_selected_field],
            m_registry->current_selected_tmp[m_registry->current_selected_field]
            );
    }

    // Update every cell if not empty
    for (entt::entity fID : m_registry->ecs.view<FarmField>()) {
        FarmField &f = m_registry->ecs.get<FarmField>(fID);
        if (!f.is_empty && !f.harvestable) {
            Plant p = m_registry->ecs.get<Plant>(f.plant);

            if (!f.is_regrowable_field) {
                // Flower growth
                if (f.grow < p.growTime) {
                    f.grow += delta;
                } else {
                    f.grow = p.growTime;
                    f.harvestable = true;
                }
            } else {
                // Tree / Machine growth
                float grow_time = p.growTime + p.regrowTime;
                if (f.grow < grow_time) {
                    f.grow += delta;
                } else {
                    if (!p.has_inventory) {
                        f.grow = grow_time;
                        f.harvestable = true;
                    } else {
                        Inventory &inv = m_registry->ecs.get<Inventory>(fID);
                        inv.amount = inv.amount + p.min_collect;
                        f.grow = p.growTime;
                    }
                }
            }


        }
    }

}

void FieldSystem::try_to_plant(std::vector<entt::entity> field, entt::entity p_id) noexcept {

    glm::vec2 mousePos = m_window->getMousePosition();
    bool pointFound = false;
    for (entt::entity fieldID : field) {
        FarmField &f = m_registry->ecs.get<FarmField>(fieldID);
        glm::vec2 pos = m_registry->ecs.get<Position>(fieldID).pos;
        glm::vec2 size = m_registry->ecs.get<Dimension>(fieldID).dim;

        // Check if mouse is over cell if no cell already found and plant is selected
        if (!pointFound && point_in_rect(mousePos, pos, size)) {
            pointFound = true;
            f.marked = true;

            if (f.is_empty && p_id != entt::null && m_registry->keys["mouse_left"]) {
                // Plant if field is empty and left click pressed
                f.is_empty = false;
                f.plant = p_id;
                f.grow = 0.0;
                f.harvestable = false;
                Plant p = m_registry->ecs.get<Plant>(p_id);
                if (p.has_inventory) {
                    Inventory inv;
                    inv.size = BigNumber(p.inventory_size);
                    inv.amount = BigNumber(0);
                    m_registry->ecs.emplace<Inventory>(fieldID, inv);
                }
            }

            if (!f.is_empty && m_registry->keys["mouse_right"]) {
                Plant p = m_registry->ecs.get<Plant>(f.plant);
                BigNumber &am = m_registry->ecs.get<Amount>(f.plant).am;
                // Try to harvest
                if (p.has_inventory) {
                    // Machine -> Add inventory am to stash and remove inv
                    Inventory inv = m_registry->ecs.get<Inventory>(fieldID);
                    am += inv.amount;
                    m_registry->ecs.remove<Inventory>(fieldID);
                    f.is_empty = true;
                } else if (f.harvestable) {
                    // No Machine -> only remove if harvestable
                    am += p.min_collect;

                    f.harvestable = false;
                    if (!f.is_regrowable_field) {
                        // Plant -> fully remove
                        f.is_empty = true;
                    } else {
                        // Tree -> Set to growTime so only regrowtime is needed
                        f.grow = p.growTime;
                    }
                }
            }

        } else {
            // If cell already found or mouse does not hover over cell
            f.marked = false;
        }



    }

}




