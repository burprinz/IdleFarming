//
// Created by luisb on 02.09.2026.
//

#include "marketplace.h"

#include "core/stats.h"
#include "helpers/finder.h"
#include "helpers/sizes.h"
#include "helpers/transaction.h"

MarketSystem MarketSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    MarketSystem self;
    self.m_window = window;
    self.m_registry = registry;
    self.m_ui = ui;
    return self;
}

void MarketSystem::deinit() noexcept {

}

void MarketSystem::reset() noexcept {
    initButtons();
}

void MarketSystem::initButtons() noexcept {
    m_ui->m_market_category_buttons.clear();
    m_ui->m_market_trade_buttons.clear();


    Registry *registry = m_registry;
    UISystem* ui = m_ui;

    glm::vec2 category_size = normal_button_size;
    glm::vec2 category_pos = normal_topleft;
    category_pos = calc_center(category_pos, category_size);


    for (entt::entity category_id: m_registry->market_category_order) {
        ItemCategory category = m_registry->ecs.get<ItemCategory>(category_id);

        TextButton button;
        button.pos = category_pos;
        button.size = category_size;
        button.text = category.display_name;
        button.id_label = category_id;
        button.on_click = [registry, category_id, ui] {
            registry->current_market_category = category_id;
            ui->update_button_states();
        };
        m_ui->m_market_category_buttons.push_back(button);

        category_pos.x += category_size.x + 0.1f;
    }


    for (entt::entity plant_id: m_registry->market_item_order) {
        std::vector<TextButton> currentB;

        TextButton buyB;
        buyB.size = trade_button_size;
        buyB.lang_label = "buy";
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
        sellB.lang_label = "sell";
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

        m_ui->m_market_trade_buttons.push_back(currentB);
    }

    m_registry->current_market_category = get_item_category_by_name(m_registry, "all");
    m_ui->update_button_states();
}

bool MarketSystem::left_click() noexcept {
    if (m_ui->check_text_button_list(m_ui->m_market_category_buttons)) {
        return true;
    }
    for (std::vector<TextButton> &buttons: m_ui->m_market_trade_buttons) {
        if (m_ui->check_text_button_list(buttons)) {
            return true;
        }
    }
    return false;
}

void MarketSystem::step(const float delta) noexcept {
    (void) delta;
}





