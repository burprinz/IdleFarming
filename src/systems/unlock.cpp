//
// Created by luisb on 30.06.2026.
//

#include "unlock.h"

#include <iostream>

#include "ui.h"
#include "core/stats.h"
#include "helpers/sizes.h"
#include "helpers/transaction.h"

UnlockSystem UnlockSystem::init(Window *window, Registry *registry, UISystem* ui) noexcept {
    UnlockSystem self;
    self.m_registry = registry;
    self.m_window = window;
    self.m_ui = ui;

    return self;
}

void UnlockSystem::deinit() noexcept {

}

void UnlockSystem::reset() noexcept {

}

void UnlockSystem::step(const float delta) noexcept {
    (void) delta;
    checkPlantUnlock();

    if (m_registry->trying_to_buy != entt::null) {

        bool bought = try_to_buy(m_registry, m_registry->trying_to_buy);

        if (bought) {
            Unlockable &u = m_registry->ecs.get<Unlockable>(m_registry->trying_to_buy);
            u.unlocked = true;

            if (m_registry->ecs.all_of<FarmHelper>(m_registry->trying_to_buy)) {
                FarmHelper &helper = m_registry->ecs.get<FarmHelper>(m_registry->trying_to_buy);
                helper.level += 1;
                helper.amount_fields = 10 + helper.level*5;
                if (helper.amount_fields >= normal_field_amount) {
                    helper.max_reached = true;
                    helper.amount_fields = normal_field_amount;
                }
            } else if (m_registry->ecs.all_of<Tool>(m_registry->trying_to_buy)) {
                StatsManager::instance().higher("tools_bought", 1);
            }
        }

        m_registry->trying_to_buy = entt::null;
    }
}


void UnlockSystem::checkPlantUnlock() noexcept {

    // Wenn keine Pflanze selected ist
    if (m_registry->current_selected_plant[m_registry->current_selected_field] == entt::null) {
        return;
    }


    entt::entity &current = m_registry->current_selected_plant[m_registry->current_selected_field];
    entt::entity &last = m_registry->last_selected_plant[m_registry->current_selected_field];

    Unlockable &unlock = m_registry->ecs.get<Unlockable>(current);

    // Wenn noch nicht unlocked wurde
    if (!unlock.unlocked) {
        BigNumber &money = m_registry->ecs.get<Amount>(m_registry->money()).am;

        // Try to unlock
        bool unlock_successful = try_to_remove(money, unlock.price);

        if (unlock_successful) {
            // Plant got unlocked, so selected plant is new plant
            unlock.unlocked = true;
            m_ui->update_button_states();
        } else {
            // Plant not unlocked, so last plant is selected
            current = last;
        }

    }

}




