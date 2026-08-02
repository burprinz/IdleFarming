//
// Created by luisb on 24.06.2026.
//

#include "ui.h"

#include "../helpers/sizes.h"
#include "../helpers/util.h"
#include "helpers/finder.h"
#include "helpers/transaction.h"

UISystem UISystem::init(Window *window, Registry *registry) noexcept {
    UISystem self;
    self.m_registry = registry;
    self.m_window = window;

    self.initWindowSwitchButtons();
    self.initFieldButton();
    self.initMarketplaceTradeButtons();
    self.initToolSmithButtons();
    self.initHelpersButton();

    return self;
}

void UISystem::initWindowSwitchButtons() noexcept {

    m_window_buttons.clear();

    Registry* registry = m_registry;


    glm::vec2 size = normal_button_size;
    glm::vec2 pos = calc_center(mode_select_buttons_topleft, normal_button_size);

    TextButton fieldButton;
    fieldButton.pos = pos;
    fieldButton.size = size;
    fieldButton.text = "Farmland";
    fieldButton.on_click = [registry] {
        registry->change_window(FIELDS);
    };
    m_window_buttons.push_back(fieldButton);

    pos.x += size.x + normal_gap;

    TextButton marketButton;
    marketButton.pos = pos;
    marketButton.size = size;
    marketButton.text = "Marketplace";
    marketButton.on_click = [registry] {
        registry->change_window(MARKETPLACE);
    };
    m_window_buttons.push_back(marketButton);

    pos.x += size.x + normal_gap;

    TextButton toolsButton;
    toolsButton.pos = pos;
    toolsButton.size = size;
    toolsButton.text = "Tool Smith";
    toolsButton.on_click = [registry] {
        registry->change_window(TOOLSMITH);
    };
    m_window_buttons.push_back(toolsButton);

    pos.x += size.x + normal_gap;

    TextButton helpersButton;
    helpersButton.pos = pos;
    helpersButton.size = size;
    helpersButton.text = "Helpers";
    helpersButton.on_click = [registry] {
        registry->change_window(HELPERS);
    };
    m_window_buttons.push_back(helpersButton);

}

void UISystem::initFieldButton() noexcept {

    m_field_plant_select_buttons.clear();
    m_field_mode_buttons.clear();

    Registry* registry = m_registry;

    glm::vec2 top_left = field_cell_topleft;
    glm::vec2 select_button_size = normal_button_size;
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

        center.x += select_button_size.x + normal_gap;
    }
}

void UISystem::initPlantLikePlantSelection(std::vector<entt::entity> order, entt::entity &last_selected, entt::entity &current_selected, std::vector<TextButton> &buttons) noexcept {
    buttons.clear();
    glm::vec2 top_left = field_plant_selection;
    glm::vec2 size = normal_name_size;
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

void UISystem::initMarketplaceTradeButtons() noexcept {

    m_market_category_buttons.clear();
    m_market_trade_buttons.clear();


    Registry* registry = m_registry;

    glm::vec2 category_size = normal_button_size;
    glm::vec2 category_pos = normal_topleft;
    category_pos = calc_center(category_pos, category_size);


    for (entt::entity category_id : m_registry->market_category_order) {
        ItemCategory category = m_registry->ecs.get<ItemCategory>(category_id);

        TextButton button;
        button.pos = category_pos;
        button.size = category_size;
        button.text = category.display_name;
        button.id_label = category_id;
        button.on_click = [registry, category_id, this] {
            registry->current_market_category = category_id;
            this->update_button_states();
        };
        m_market_category_buttons.push_back(button);

        category_pos.x += category_size.x + 0.1f;
    }


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

    glm::vec2 size = tool_item_buy_size;
    glm::vec2 top_left = normal_topleft;
    glm::vec2 center = calc_center(top_left, size);

    // Create Button for every Tool
    for (entt::entity tool_id : m_registry->tool_order) {
        TextButton toolB;
        toolB.pos = center;
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
                    registry->current_collect_tools[t.field_type] = tool_id;
                }
            }
        };

        m_toolsmith_buy_buttons.push_back(toolB);
        center.y -= size.y + normal_gap;
    }

}

void UISystem::initHelpersButton() noexcept {
    m_helper_buy_buttons.clear();

    glm::vec2 top_left = normal_topleft;
    glm::vec2 size = normal_name_size;
    glm::vec2 center = calc_center(top_left, size);

    for (entt::entity helper_id : m_registry->helpers_order) {
        FarmHelper fm = m_registry->ecs.get<FarmHelper>(helper_id);

        TextButton helperB;

        helperB.text = fm.name;
        helperB.pos = center;
        helperB.size = size;
        helperB.id_label = helper_id;
        helperB.on_click = [] {
            std::cout<<"TRYING TO BUY HELPER"<<std::endl;
        };
        m_helper_buy_buttons.push_back(helperB);

        center.y -= size.y + normal_gap;
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
                found = check_text_button_list(m_market_category_buttons);
                if (found) {
                    return;
                }
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
            } else if (m_registry->window() == HELPERS) {
                found = check_text_button_list(m_helper_buy_buttons);
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

        selection_found = update_text_button_list(m_market_category_buttons, mousePos, selection_found, false);

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
    } else if (m_registry->window() == HELPERS) {

        // Field buttons
        selection_found = update_text_button_list(m_helper_buy_buttons, mousePos, selection_found, false);

        (void) selection_found;

    }
}

void UISystem::update_button_states() noexcept {


    glm::vec2 size = trade_button_size;
    glm::vec2 pos = trade_buttons_topleft;
    pos = calc_center(pos, size);

    /**
     * Update Market Trade Buttons
     */
    for (std::vector<TextButton> &buttons : m_market_trade_buttons) {
        entt::entity item_id = buttons[0].id_label;

        bool should_be_visible = m_registry->ecs.get<Unlockable>(item_id).unlocked;

        // If not unlocked OR Not in current category
        if (!should_be_visible || !is_in_list(m_registry->ecs.get<ItemCategory>(m_registry->current_market_category).items, item_id)) {
            buttons[0].visible = false;
            buttons[1].visible = false;
            continue;
        }

        float x1 = pos.x + normal_name_size.x + trade_item_stash_size.x + 0.2 + trade_button_size.y + 0.1;
        float x2 = x1 + trade_button_size.x + 0.1;

        buttons[0].visible = true;
        buttons[1].visible = true;

        buttons[0].pos = {x1, pos.y};
        buttons[1].pos = {x2, pos.y};

        pos.y -= trade_button_size.y + 0.1;
    }

}


std::vector<TextButton> UISystem::window_buttons() {
    return m_window_buttons;
}


std::vector<TextButton> UISystem::get_current_text_buttons() noexcept {
    return std::vector<TextButton>();
}



