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

}

void UISystem::initFieldButton() noexcept {
    m_field_plant_select_buttons.clear();
    m_field_flower_select_buttons.clear();
    m_field_mode_buttons.clear();

    Registry* registry = m_registry;

    glm::vec2 top_left = {0.1, config::CAMERA_VIEW_HEIGHT-0.65};
    glm::vec2 select_button_size = {1.0, 0.5};
    glm::vec2 center = calc_center(top_left, select_button_size);

    TextButton plantFieldB;
    plantFieldB.text = "Plant";
    plantFieldB.pos = center;
    plantFieldB.size = select_button_size;
    plantFieldB.on_click = [registry] {
        registry->current_selected_field = PLANT_FIELD;
    };
    m_field_mode_buttons.push_back(plantFieldB);

    center += glm::vec2{select_button_size.x + 0.1, 0};

    TextButton flowerFieldB;
    flowerFieldB.text = "Flower";
    flowerFieldB.pos = center;
    flowerFieldB.size = select_button_size;
    flowerFieldB.on_click = [registry] {
        registry->current_selected_field = FLOWER_FIELD;
    };
    m_field_mode_buttons.push_back(flowerFieldB);


    /**
     * Plant selection
     */
    top_left = {6.0, config::CAMERA_VIEW_HEIGHT-1.2};
    center = calc_center(top_left, plant_name_size);
    int c = 0;
    for (entt::entity plant_id : m_registry->plant_order) {
        float y = center.y - c*(plant_name_size.y + 0.1);
        TextButton plantB;
        plantB.pos = {center.x, y};
        plantB.size = plant_name_size;
        plantB.id_label = plant_id;
        plantB.on_click = [registry, plant_id] {
            registry->last_selected_plant = registry->current_selected_plant;
            registry->current_selected_plant = plant_id;
        };
        m_field_plant_select_buttons.push_back(plantB);
        c += 1;
    }

    /**
     * Flower selection
     */
    c = 0;
    for (entt::entity flower_id : m_registry->flower_order) {
        float y = center.y - c*(plant_name_size.y + 0.1);
        TextButton flowerB;
        flowerB.pos = {center.x, y};
        flowerB.size = plant_name_size;
        flowerB.id_label = flower_id;
        flowerB.on_click = [registry, flower_id] {
            registry->last_selected_plant = registry->current_selected_plant;
            registry->current_selected_plant = flower_id;
        };
        m_field_flower_select_buttons.push_back(flowerB);
        c += 1;
    }

}

void UISystem::initMarketplaceTradeButtons() noexcept {
    glm::vec2 trade_button_size = {0.7, 0.5};

    m_market_trade_buttons.clear();

    Registry* registry = m_registry;

    int c = 0;
    for (entt::entity plant_id : m_registry->plant_order) {
        std::vector<TextButton> currentB;

        float y = trade_buttons_topleft.y - c * (trade_button_size.y + 0.1);
        float x1 = trade_buttons_topleft.x + plant_name_size.x + market_stash_size.x + 0.2 + trade_button_size.x/2.0f;
        float x2 = x1 + trade_button_size.x + 0.1;
        TextButton buyB;
        buyB.pos = {x1, y};
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
        sellB.pos = {x2, y};
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
        c += 1;
    }

    std::reverse(m_market_trade_buttons.begin(), m_market_trade_buttons.end());

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

                if (m_registry->current_selected_field == PLANT_FIELD) {
                    found = check_text_button_list(m_field_plant_select_buttons);
                    if (found) {
                        return;
                    }
                } else if (m_registry->current_selected_field == FLOWER_FIELD) {
                    found = check_text_button_list(m_field_flower_select_buttons);
                    if (found) {
                        return;
                    }
                }


            } else if (m_registry->window() == MARKETPLACE) {
                for (std::vector<TextButton> &buttons : m_market_trade_buttons) {
                    found = check_text_button_list(buttons);
                    if (found) {
                        return;
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

    // Window switch buttons
    selection_found = update_text_button_list(m_window_buttons, mousePos, selection_found);

    if (m_registry->window() == FIELDS) {

        // Field buttons
        selection_found = update_text_button_list(m_field_mode_buttons, mousePos, selection_found);
        if (m_registry->current_selected_field == PLANT_FIELD) {
            selection_found = update_text_button_list(m_field_plant_select_buttons, mousePos, selection_found);
        } else if (m_registry->current_selected_field == FLOWER_FIELD) {
            selection_found = update_text_button_list(m_field_flower_select_buttons, mousePos, selection_found);
        }
        (void) selection_found;


    } else if (m_registry->window() == MARKETPLACE) {

        // Marketplace buy / sell
        for (std::vector<TextButton> &buttons : m_market_trade_buttons) {
            selection_found = update_text_button_list(buttons, mousePos, selection_found);
        }
    }
}


std::vector<TextButton> UISystem::window_buttons() {
    return m_window_buttons;
}


std::vector<TextButton> UISystem::get_current_text_buttons() noexcept {
    return std::vector<TextButton>();
}



