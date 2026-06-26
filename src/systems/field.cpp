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

    glm::vec2 mousePos = m_window->getMousePosition();

    bool pointFound = false;
    for (entt::entity fieldID : m_registry->ecs.view<FarmField>()) {
        FarmField &f = m_registry->ecs.get<FarmField>(fieldID);
        if (m_registry->window() == FIELDS) {
            glm::vec2 pos = m_registry->ecs.get<Position>(fieldID).pos;
            glm::vec2 size = m_registry->ecs.get<Dimension>(fieldID).dim;
            if (!pointFound && point_in_rect(mousePos, pos, size)) {
                pointFound = true;
                f.marked = true;
                if (f.is_empty && m_registry->keys["mouse_left"]) {
                    f.is_empty = false;
                    f.plant = m_registry->plants[m_registry->current_selected_plant];
                    f.grow = 0.0;
                    f.harvestable = false;
                } else if (f.harvestable && m_registry->keys["mouse_right"]) {
                    BigNumber &am = m_registry->ecs.get<Amount>(f.plant).am;
                    am += 1;

                    f.harvestable = false;
                    f.is_empty = true;
                }
            } else {
                f.marked = false;
            }
        }

        if (!f.is_empty && !f.harvestable) {
            Plant p = m_registry->ecs.get<Plant>(f.plant);
            if (f.grow < p.growTime) {
                f.grow += delta;
            } else {
                f.grow = p.growTime;
                f.harvestable = true;
            }
        }

    }

}



