//
// Created by luisb on 30.06.2026.
//

#include "unlock.h"

#include <iostream>

#include "ui.h"
#include "core/operations.h"

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




