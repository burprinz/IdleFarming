//
// Created by luisb on 24.06.2026.
//

#include "ui.h"

#include "core/operations.h"
#include "core/sizes.h"
#include "core/util.h"

UISystem UISystem::init(Window *window, Registry *registry) noexcept {
    UISystem self;
    self.m_registry = registry;
    self.m_window = window;

    self.initWindowSwitchButtons();
    self.initFieldButton();
    self.initMarketplaceTradeButtons();
    self.initToolSmithButtons();

    return self;
}

void UISystem::initWindowSwitchButtons() noexcept {

    m_window_buttons.clear();

    Registry* registry = m_registry;

    TextButton fieldButton;
    fieldButton.pos = {0.6, config::CAMERA_VIEW_HEIGHT-0.35};
    fieldButton.size = {1.0, 0.5};
    fieldButton.text = "Farmland";
    fieldButton.on_click = [registry] {
        registry->change_window(FIELDS);
    };
    m_window_buttons.push_back(fieldButton);

    TextButton geneLabButton;
    geneLabButton.pos = {1.7, config::CAMERA_VIEW_HEIGHT-0.35};
    geneLabButton.size = {1.0, 0.5};
    geneLabButton.text = "Gene Lab";
    geneLabButton.on_click = [registry] {
        registry->change_window(GENE_LAB);
    };
    m_window_buttons.push_back(geneLabButton);

    TextButton marketButton;
    marketButton.pos = {2.8, config::CAMERA_VIEW_HEIGHT-0.35};
    marketButton.size = {1.0, 0.5};
    marketButton.text = "Marketplace";
    marketButton.on_click = [registry] {
        registry->change_window(MARKETPLACE);
    };
    m_window_buttons.push_back(marketButton);

    TextButton toolsButton;
    toolsButton.pos = {3.9, config::CAMERA_VIEW_HEIGHT-0.35};
    toolsButton.size = {1.0, 0.5};
    toolsButton.text = "Tool Smith";
    toolsButton.on_click = [registry] {
        registry->change_window(TOOLSMITH);
    };
    m_window_buttons.push_back(toolsButton);

}

void UISystem::initFieldButton() noexcept {

    m_field_plant_select_buttons.clear();
    m_field_mode_buttons.clear();

    Registry* registry = m_registry;

    glm::vec2 top_left = {0.1, config::CAMERA_VIEW_HEIGHT-0.65};
    glm::vec2 select_button_size = {1.0, 0.5};
    glm::vec2 center = calc_center(top_left, select_button_size);


    /**
     * Field selection
     */

    for (size_t field_id = 0; field_id < m_registry->field_order.size(); field_id++) {
        Field f = m_registry->ecs.get<Field>(m_registry->field_order[field_id]);

        TextButton fieldB;
        fieldB.text = f.display_name;
        fieldB.pos = center;
        fieldB.size = select_button_size;
        fieldB.on_click = [registry, field_id] {
            registry->current_selected_field = field_id;
        };
        m_field_mode_buttons.push_back(fieldB);

        std::vector<TextButton> plant_select_buttons;
        initPlantLikePlantSelection(m_registry->plant_order[field_id], m_registry->last_selected_plant[field_id], m_registry->current_selected_plant[field_id], plant_select_buttons);
        m_field_plant_select_buttons.push_back(plant_select_buttons);

        center += glm::vec2{select_button_size.x + 0.1, 0};
    }

}

void UISystem::initPlantLikePlantSelection(std::vector<entt::entity> order, entt::entity &last_selected, entt::entity &current_selected, std::vector<TextButton> &buttons) noexcept {
    buttons.clear();
    glm::vec2 top_left = {6.0, config::CAMERA_VIEW_HEIGHT-1.2};
    glm::vec2 center = calc_center(top_left, plant_name_size);
    int c = 0;
    for (entt::entity id : order) {
        float y = center.y - c*(plant_name_size.y + 0.1);
        TextButton plantB;
        plantB.pos = {center.x, y};
        plantB.size = plant_name_size;
        plantB.id_label = id;
        plantB.on_click = [&last_selected, &current_selected, id] {
            last_selected = current_selected;
            current_selected = id;
        };
        buttons.push_back(plantB);
        c += 1;
    }
}

void UISystem::initMarketplaceTradeButtons() noexcept {

    m_market_trade_buttons.clear();

    Registry* registry = m_registry;

    for (entt::entity plant_id : m_registry->market_item_order) {
        std::vector<TextButton> currentB;

        TextButton buyB;
        buyB.size = trade_button_size;
        buyB.text = "Buy";
        buyB.id_label = plant_id;
        buyB.on_click = [registry, plant_id] {
            BigNumber &money = registry->ecs.get<Amount>(registry->money()).am;
            BigNumber price = registry->ecs.get<Price>(plant_id).buy * registry->market_multiplier;

            bool success = try_to_remove(money, price);
            if (success) {
                BigNumber &amount = registry->ecs.get<Amount>(plant_id).am;
                amount = amount + registry->market_multiplier;
            }
        };
        currentB.push_back(buyB);

        TextButton sellB;
        sellB.size = trade_button_size;
        sellB.text = "Sell";
        sellB.id_label = plant_id;
        sellB.on_click = [registry, plant_id] {
            BigNumber price = registry->ecs.get<Price>(plant_id).sell;
            BigNumber &money = registry->ecs.get<Amount>(registry->money()).am;
            BigNumber &amount = registry->ecs.get<Amount>(plant_id).am;
            if (amount > BigNumber(0)) {
                BigNumber multiplier = registry->market_multiplier;
                if (amount < multiplier) {
                    multiplier = amount;
                }
                price = price * multiplier;
                money = money + price;
                amount = amount - multiplier;
            }
        };
        currentB.push_back(sellB);

        m_market_trade_buttons.push_back(currentB);
    }



    update_button_states();
}

void UISystem::initToolSmithButtons() noexcept {
    m_toolsmith_buy_buttons.clear();

    Registry* registry = m_registry;

    glm::vec2 size = {0.5, 0.5};

    float button_gap = size.x + 0.1;


    glm::vec2 top_left = {0.1, config::CAMERA_VIEW_HEIGHT-0.7};
    glm::vec2 center = calc_center(top_left, size);

    // Create Button for every Tool
    int c = 0;
    for (entt::entity tool_id : m_registry->tool_order) {
        float y = center.y - c * button_gap;
        TextButton toolB;
        toolB.pos = {center.x, y};
        toolB.size = size;
        toolB.id_label = tool_id;
        toolB.on_click = [registry, tool_id] {
            bool unlocked = registry->ecs.get<Unlockable>(tool_id).unlocked;

            if (!unlocked) {
                registry->trying_to_buy = tool_id;
            } else {
                Tool t = registry->ecs.get<Tool>(tool_id);

                if (t.usage == PLANTING) {
                    registry->current_plant_tools[t.field_type] = tool_id;
                } else if (t.usage == HARVESTING) {
                    registry->current_harvest_tools[t.field_type] = tool_id;
                } else if (t.usage == COLLECTING) {
                    // TODO
                }
            }
        };

        m_toolsmith_buy_buttons.push_back(toolB);
        c++;
    }

}



void UISystem::deinit() noexcept {

}

void UISystem::reset() noexcept {

}

void UISystem::onMouseClickCallback(GLFWwindow *window, int button, int action, int mods) noexcept {
    (void) window;
    (void) (button + action + mods);
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_1) {
            bool found = check_text_button_list(m_window_buttons);
            if (found) {
                return;
            }
            if (m_registry->window() == FIELDS) {
                found = check_text_button_list(m_field_mode_buttons);
                if (found) {
                    return;
                }

                found = check_text_button_list(m_field_plant_select_buttons[m_registry->current_selected_field]);
                if (found) {
                    return;
                }
            } else if (m_registry->window() == MARKETPLACE) {
                for (std::vector<TextButton> &buttons : m_market_trade_buttons) {
                    found = check_text_button_list(buttons);
                    if (found) {
                        return;
                    }
                }
            } else if (m_registry->window() == TOOLSMITH) {
                found = check_text_button_list(m_toolsmith_buy_buttons);
                if (found) {
                    return;
                }
            }
        }
    }
}


void UISystem::step(const float delta) noexcept {
    (void) delta;

    glm::vec2 mousePos = m_window->getMousePosition();
    bool selection_found = false;

    // Window switch buttons
    selection_found = update_text_button_list(m_window_buttons, mousePos, selection_found, false);

    if (m_registry->window() == FIELDS) {

        // Field buttons
        selection_found = update_text_button_list(m_field_mode_buttons, mousePos, selection_found, false);

        selection_found = update_text_button_list(m_field_plant_select_buttons[m_registry->current_selected_field], mousePos, selection_found, false);

        (void) selection_found;


    } else if (m_registry->window() == MARKETPLACE) {

        // Marketplace buy / sell
        for (std::vector<TextButton> &buttons : m_market_trade_buttons) {
            selection_found = update_text_button_list(buttons, mousePos, selection_found, true);
        }
    } else if (m_registry->window() == TOOLSMITH) {

        m_registry->current_hover_tool = entt::null;

        for (TextButton &b : m_toolsmith_buy_buttons) {
            if (!selection_found && b.visible && point_in_rect(mousePos, b.pos, b.size)) {
                b.selected = true;
                selection_found = true;
                m_registry->current_hover_tool = b.id_label;
            } else {
                b.selected = false;
            }
        }
    }
}

void UISystem::update_button_states() noexcept {

    /**
     * Update Market Trade Buttons
     */
    int c = 0;
    for (std::vector<TextButton> &buttons : m_market_trade_buttons) {
        bool should_be_visible = m_registry->ecs.get<Unlockable>(buttons[0].id_label).unlocked;


        float x1 = trade_buttons_topleft.x + plant_name_size.x + market_stash_size.x + 0.2 + trade_button_size.x/2.0f + trade_button_size.y + 0.1;
        float x2 = x1 + trade_button_size.x + 0.1;
        if (should_be_visible) {

            buttons[0].visible = true;
            buttons[1].visible = true;

            float y = trade_buttons_topleft.y - c * (trade_button_size.y + 0.1);

            buttons[0].pos = {x1, y};
            buttons[1].pos = {x2, y};

            c += 1;
        }

    }

}


std::vector<TextButton> UISystem::window_buttons() {
    return m_window_buttons;
}


std::vector<TextButton> UISystem::get_current_text_buttons() noexcept {
    return std::vector<TextButton>();
}



