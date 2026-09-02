//
// Created by luisb on 02.09.2026.
//

#include "toolsmith.h"

#include "helpers/sizes.h"

ToolSystem ToolSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    ToolSystem self;
    self.m_window = window;
    self.m_registry = registry;
    self.m_ui = ui;
    return self;
}

void ToolSystem::deinit() noexcept {

}

void ToolSystem::reset() noexcept {
    initButtons();
}

void ToolSystem::initButtons() noexcept {
    m_ui->m_toolsmith_buy_buttons.clear();

    Registry *registry = m_registry;

    glm::vec2 size = tool_item_buy_size;
    glm::vec2 top_left = normal_topleft;
    glm::vec2 center = calc_center(top_left, size);

    // Create Button for every Tool
    for (entt::entity tool_id: m_registry->tool_order) {
        TextButton toolB;
        toolB.pos = center;
        toolB.size = size;
        toolB.id_label = tool_id;
        toolB.on_click = [registry, tool_id] {
            bool unlocked = registry->ecs.get<Unlockable>(tool_id).unlocked;

            if (!unlocked) {
                registry->trying_to_buy = tool_id;
            } else {
                activate_tool(registry, tool_id);
            }
        };

        m_ui->m_toolsmith_buy_buttons.push_back(toolB);
        center.y -= size.y + normal_gap;
    }
}

bool ToolSystem::left_click() noexcept {
    return m_ui->check_text_button_list(m_ui->m_toolsmith_buy_buttons);
}

void ToolSystem::step(const float delta) noexcept {
    (void) delta;
}




