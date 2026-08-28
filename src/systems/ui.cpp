//
// Created by luisb on 24.06.2026.
//

#include "ui.h"

#include <bits/regex_constants.h>

#include "../helpers/sizes.h"
#include "../helpers/util.h"
#include "core/stats.h"
#include "helpers/finder.h"
#include "helpers/transaction.h"

UISystem UISystem::init(Window *window, Registry *registry) noexcept {
    UISystem self;
    self.m_registry = registry;
    self.m_window = window;
    return self;
}

void UISystem::initBaseButtons() noexcept {
    message_popup_close_buttons.clear();

    TextButton singleMessageCLose;
    singleMessageCLose.text = "Close";
    singleMessageCLose.pos = message_popup_close_button_center;
    singleMessageCLose.size = message_popup_close_button_size;
    singleMessageCLose.on_click = [this] {
        this->closeCurrentMessagePopup();
    };
    message_popup_close_buttons.push_back(singleMessageCLose);

    TextButton allMessageClose;
    allMessageClose.text = "Close all (0)";
    allMessageClose.pos = message_popup_close_all_button_center;
    allMessageClose.size = message_popup_close_button_size;
    allMessageClose.on_click = [this] {
        this->cloaseAllMessagePopups();
    };
    message_popup_close_buttons.push_back(allMessageClose);
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

    pos.x += size.x + normal_gap;

    TextButton achievementsButton;
    achievementsButton.pos = pos;
    achievementsButton.size = size;
    achievementsButton.text = "Achievements";
    achievementsButton.on_click = [registry] {
        registry->change_window(ACHIEVEMENTS);
    };
    m_window_buttons.push_back(achievementsButton);

    pos.x += size.x + normal_gap;

    TextButton gableButton;
    gableButton.pos = pos;
    gableButton.size = size;
    gableButton.text = "Gamble";
    gableButton.on_click = [registry] {
        registry->change_window(GAMBLING);
    };
    m_window_buttons.push_back(gableButton);

}

void UISystem::initFieldButton() noexcept {

    m_field_plant_select_buttons.clear();
    m_field_mode_buttons.clear();
    m_field_helper_buttons.clear();
    m_field_helper_config_buttons.clear();

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
        fieldB.pos = center;
        fieldB.size = select_button_size;
        fieldB.on_click = [registry, field_id, field_entity] {
            if (registry->field_mode == FIELD_FARMING) {
                registry->current_selected_field = field_id;
            } else if (registry->field_mode == FIELD_HELPER_CONFIGURATION) {
                if (registry->ecs.get<Unlockable>(registry->field_helper[field_entity]).unlocked) {
                    registry->current_selected_field = field_id;
                }
            }
        };
        m_field_mode_buttons.push_back(fieldB);

        // Plant Selection
        std::vector<TextButton> plant_select_buttons;
        initPlantLikePlantSelection(m_registry->plant_order[field_id], m_registry->last_selected_plant[field_id], m_registry->current_selected_plant[field_id], plant_select_buttons);
        m_field_plant_select_buttons.push_back(plant_select_buttons);

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
        m_field_helper_buttons.push_back(helper_buttons);

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
    m_field_helper_config_buttons.push_back(helper_back_button);
    // TODO CONFIGURATION
}

void UISystem::initPlantLikePlantSelection(std::vector<entt::entity> order, entt::entity &last_selected, entt::entity &current_selected, std::vector<TextButton> &buttons) noexcept {
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
                StatsManager::instance().higher("items_bought", registry->market_multiplier);
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
                StatsManager::instance().higher("items_sold", multiplier);
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
                activate_tool(registry, tool_id);
            }
        };

        m_toolsmith_buy_buttons.push_back(toolB);
        center.y -= size.y + normal_gap;
    }

}

void UISystem::initHelpersButton() noexcept {
    m_helper_buy_buttons.clear();

    Registry* registry = m_registry;

    glm::vec2 top_left = normal_topleft;
    glm::vec2 size = medium_button_size;
    glm::vec2 center = calc_center(top_left, size);

    for (entt::entity helper_id : m_registry->helpers_order) {
        FarmHelper fm = m_registry->ecs.get<FarmHelper>(helper_id);

        TextButton helperB;

        helperB.text = fm.name;
        helperB.pos = center;
        helperB.size = size;
        helperB.id_label = helper_id;
        helperB.on_click = [registry, helper_id] {
            bool unlocked = registry->ecs.get<Unlockable>(helper_id).unlocked;
            (void) unlocked;

            bool max_reached = registry->ecs.get<FarmHelper>(helper_id).max_reached;
            if (!max_reached) {
                registry->trying_to_buy = helper_id;
            }
        };
        m_helper_buy_buttons.push_back(helperB);

        center.y -= size.y + normal_gap;
    }
}

void UISystem::initGambleButtons() noexcept {
    m_gamble_mode_select_buttons.clear();
    m_blackjack_bet_buttons.clear();
    Registry* registry = m_registry;

    glm::vec2 top_left = field_cell_topleft;
    glm::vec2 size = normal_button_size;
    glm::vec2 center = calc_center(top_left, size);

    TextButton blackjackB;
    blackjackB.text = "Blackjack";
    blackjackB.pos = center;
    blackjackB.size = size;
    blackjackB.on_click = [registry] {
        if (registry->gamble_mode != BLACKJACK) {
            registry->gamble_stage = BLACKJACK_BETTING;
        }
        registry->gamble_mode = BLACKJACK;
    };
    m_gamble_mode_select_buttons.push_back(blackjackB);

    size = normal_long_button_size;
    top_left = {normal_gap, normal_gap + normal_button_size.y};
    center = calc_center(top_left, size);

    size_t idx = m_blackjack_bet_buttons.size();
    TextButton multiplicatorB;
    multiplicatorB.text = "Bet Amount (1)";
    multiplicatorB.pos = center;
    multiplicatorB.size = size;
    multiplicatorB.on_click = [registry, this, idx] {
        int current_mult = registry->bet_multiplicator;
        int next_mult = current_mult*10;
        if (next_mult > 1000) {
            next_mult = 1;
        }
        registry->bet_multiplicator = next_mult;
        this->m_blackjack_bet_buttons[idx].text = "Bet Amount (" + std::to_string(registry->bet_multiplicator) + ")";
    };
    m_registry->bet_multiplicator = 1;
    m_blackjack_bet_buttons.push_back(multiplicatorB);

    float x_middle = config::CAMERA_VIEW_WIDTH / 2.0f;
    (void) x_middle;
    // TODO Unten Mitte bet; links weniger, rechts mehr button; ganz links multiplikator (1,10,100,1000)
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
            bool found = false;
            if (messagePopupAvailable()) {
                found = check_text_button_list(message_popup_close_buttons);
                if (found) {
                    return;
                }
            }
            found = check_text_button_list(m_window_buttons);
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

                if (m_registry->field_mode == FIELD_FARMING) {
                    // Planting / Harvesting
                    found = check_text_button_list(m_field_helper_buttons[m_registry->current_selected_field]);
                    if (found) {
                        return;
                    }

                } else if (m_registry->field_mode == FIELD_HELPER_CONFIGURATION) {
                    // Helper Config
                    found = check_text_button_list(m_field_helper_config_buttons);
                    if (found) {
                        return;
                    }

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
            } else if (m_registry->window() == GAMBLING) {
                found = check_text_button_list(m_gamble_mode_select_buttons);
                if (found) {
                    return;
                }

                if (m_registry->gamble_mode == BLACKJACK) {
                    if (m_registry->gamble_stage == BLACKJACK_BETTING) {
                        found = check_text_button_list(m_blackjack_bet_buttons);
                        if (found) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

void UISystem::step(const float delta) noexcept {
    (void) delta;

    glm::vec2 mousePos = m_window->getMousePosition();
    bool selection_found = false;

    // Popup Buttons
    if (messagePopupAvailable()) {
        updateMessagePopupButtons();
        selection_found = update_text_button_list(message_popup_close_buttons, mousePos, selection_found, false);
    }

    // Window switch buttons
    selection_found = update_text_button_list(m_window_buttons, mousePos, selection_found, false);

    if (m_registry->window() == FIELDS) {
        // Field buttons
        selection_found = update_text_button_list(m_field_mode_buttons, mousePos, selection_found, false);

        if (m_registry->field_mode == FIELD_FARMING) {
            // Planting / Harvesting
            selection_found = update_text_button_list(m_field_plant_select_buttons[m_registry->current_selected_field], mousePos, selection_found, false);

            selection_found = update_text_button_list(m_field_helper_buttons[m_registry->current_selected_field], mousePos, selection_found, true);
        } else if (m_registry->field_mode == FIELD_HELPER_CONFIGURATION) {
            // Helper Config
            selection_found = update_text_button_list(m_field_plant_select_buttons[m_registry->current_selected_field], mousePos, selection_found, false, helper_config_field_offset);

            selection_found = update_text_button_list(m_field_helper_config_buttons, mousePos, selection_found, false);
        }

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

    } else if (m_registry->window() == GAMBLING) {
        selection_found = update_text_button_list(m_gamble_mode_select_buttons, mousePos, selection_found, false);

        if (m_registry->gamble_mode == BLACKJACK) {
            if (m_registry->gamble_stage == BLACKJACK_BETTING) {
                selection_found = update_text_button_list(m_blackjack_bet_buttons, mousePos, selection_found, false);
            }
        }
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

        float x1 = pos.x + medium_button_size.x + trade_item_stash_size.x + 0.2 + trade_button_size.y + 0.1;
        float x2 = x1 + trade_button_size.x + 0.1;

        buttons[0].visible = true;
        buttons[1].visible = true;

        buttons[0].pos = {x1, pos.y};
        buttons[1].pos = {x2, pos.y};

        pos.y -= trade_button_size.y + 0.1;
    }

}


