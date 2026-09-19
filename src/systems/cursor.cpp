//
// Created by luisb on 19.09.2026.
//

#include "cursor.h"

#include "helpers/sizes.h"

CursorSystem CursorSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    CursorSystem self;
    self.m_window = window;
    self.m_registry = registry;
    self.m_ui = ui;
    return self;
}

void CursorSystem::deinit() noexcept {

}

void CursorSystem::reset() noexcept {

}

void CursorSystem::step(const float delta) noexcept {
    (void) delta;

    glm::vec2 mouse_pos = m_window->getMousePosition();
    bool inside_fields = point_in_rect(mouse_pos, full_field_center, field_tool_cursor_size);
    bool valid_tile = m_registry->current_hover_field_tile != entt::null;

    if (inside_fields && valid_tile) {
        normal_cursor_active = false;
        FarmField tile = m_registry->ecs.get<FarmField>(m_registry->current_hover_field_tile);
        Field field = m_registry->ecs.get<Field>(tile.parent_field);

        std::string tool = "Hoe";
        if (field.plant_type == NORMAL_PLANT) {
            if (tile.harvestable || m_registry->keys["shift"]) {
                tool = "Scythe";
            } else {
                tool = "Hoe";
            }
        } else if (field.plant_type == COLLECT_PLANT) {
            if (m_registry->keys["shift"]) {
                tool = "Axe";
            } else if (tile.harvestable) {
                tool = "Basket";
            } else {
                tool = "Hoe";
            }
        } else if (field.plant_type == INVENTORY_PLANT) {
            if (m_registry->keys["shift"]) {
                tool = "Demolition";
            } else if (tile.plant != entt::null) {
                tool = "LKW";
            } else {
                tool = "Crane";
            }
        }
        if (current_tool_name != tool) {
            current_tool_name = tool;
            std::cout << "Switch to " << current_tool_name << std::endl;
        }
    } else if (!normal_cursor_active) {
        normal_cursor_active = true;
        current_tool_name = "";
        std::cout << "Switch to normal" << std::endl;
    }

}
