//
// Created by luisb on 24.06.2026.
//

#include "ui.h"

#include "core/sizes.h"
#include "core/util.h"

UISystem UISystem::init(Window *window, Registry *registry) noexcept {
    UISystem self;
    self.m_registry = registry;
    self.m_window = window;

    self.initWindowSwitchButtons();
    self.initPlantSelectButton();
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

void UISystem::initPlantSelectButton() noexcept {
    this->m_field_plant_select_buttons.clear();

    Registry* registry = m_registry;

    float x_start = 6.0;
    float y_start = config::CAMERA_VIEW_HEIGHT-1.2;
    float x_center = x_start + plant_name_size.x / 2.0;
    float y_center = y_start - plant_name_size.y / 2.0;

    int c = 0;
    for (std::string plant_name : m_registry->plant_ids) {
        float y = y_center - c*(plant_name_size.y + 0.1);
        TextButton plantB;
        plantB.pos = {x_center, y};
        plantB.size = plant_name_size;
        plantB.text = plant_name;
        plantB.on_click = [registry, plant_name] {

            entt::entity id = registry->plants[plant_name];
            Unlockable &u = registry->ecs.get<Unlockable>(id);
            if (u.unlocked) {
                registry->current_selected_plant = plant_name;
            } else {
                BigNumber &money = registry->ecs.get<Amount>(registry->money()).am;
                if (u.price <= money) {
                    money = money - u.price;
                    u.unlocked = true;
                    registry->current_selected_plant = plant_name;
                }
            }
        };
        this->m_field_plant_select_buttons.push_back(plantB);
        c += 1;
    }

}

void UISystem::initMarketplaceTradeButtons() noexcept {
    glm::vec2 trade_button_size = {0.7, 0.5};

    m_market_trade_buttons.clear();

    Registry* registry = m_registry;

    int c = 0;
    for (std::string plant_name : m_registry->plant_ids) {
        std::vector<TextButton> currentB;

        float y = trade_buttons_topleft.y - c * (trade_button_size.y + 0.1);
        float x1 = trade_buttons_topleft.x + plant_name_size.x + market_stash_size.x + 0.2 + trade_button_size.x/2.0f;
        float x2 = x1 + trade_button_size.x + 0.1;
        TextButton buyB;
        buyB.pos = {x1, y};
        buyB.size = trade_button_size;
        buyB.text = "Buy";
        buyB.label = plant_name;
        buyB.on_click = [registry, plant_name] {
            entt::entity p_id = registry->plants[plant_name];
            BigNumber price = registry->ecs.get<Price>(p_id).buy;
            BigNumber &money = registry->ecs.get<Amount>(registry->money()).am;
            BigNumber &amount = registry->ecs.get<Amount>(p_id).am;
            price = price * registry->market_multiplier;
            if (money >= price) {
                money = money - price;
                amount = amount + registry->market_multiplier;
            }
        };
        currentB.push_back(buyB);

        TextButton sellB;
        sellB.pos = {x2, y};
        sellB.size = trade_button_size;
        sellB.text = "Sell";
        sellB.label = plant_name;
        sellB.on_click = [registry, plant_name] {
            entt::entity p_id = registry->plants[plant_name];
            BigNumber price = registry->ecs.get<Price>(p_id).sell;
            BigNumber &money = registry->ecs.get<Amount>(registry->money()).am;
            BigNumber &amount = registry->ecs.get<Amount>(p_id).am;
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
            for (TextButton &b : m_window_buttons) {
                if (b.selected) {
                    b.on_click();
                    return;
                }
            }
            if (m_registry->window() == FIELDS) {
                for (TextButton &b : m_field_plant_select_buttons) {
                    if (b.selected) {
                        b.on_click();
                        return;
                    }
                }
            } else if (m_registry->window() == MARKETPLACE) {
                for (std::vector<TextButton> &buttons : m_market_trade_buttons) {
                    for (TextButton &b : buttons) {
                        if (b.selected) {
                            b.on_click();
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
    for (TextButton &button : m_window_buttons) {
        if (!selection_found && point_in_rect(mousePos, button.pos, button.size)) {
            button.selected = true;
            selection_found = true;
        } else {
            button.selected = false;
        }
    }

    if (m_registry->window() == FIELDS) {
        for (TextButton &b : m_field_plant_select_buttons) {
            if (!selection_found && point_in_rect(mousePos, b.pos, b.size)) {
                b.selected = true;
                selection_found = true;
            } else {
                b.selected = false;
            }
        }
    } else if (m_registry->window() == MARKETPLACE) {
        for (std::vector<TextButton> &buttons : m_market_trade_buttons) {
            for (TextButton &b : buttons) {
                if (!selection_found && point_in_rect(mousePos, b.pos, b.size)) {
                    b.selected = true;
                    selection_found = true;
                } else {
                    b.selected = false;
                }
            }
        }
    }
}


std::vector<TextButton> UISystem::window_buttons() {
    return m_window_buttons;
}


std::vector<TextButton> UISystem::get_current_text_buttons() noexcept {
    return std::vector<TextButton>();
}



