//
// Created by luisb on 02.09.2026.
//

#include "village.h"

#include "helpers/sizes.h"

VillageSystem VillageSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    VillageSystem self;
    self.m_window = window;
    self.m_registry = registry;
    self.m_ui = ui;
    return self;
}

void VillageSystem::deinit() noexcept {

}

void VillageSystem::reset() noexcept {
    initButtons();
}

void VillageSystem::initButtons() noexcept {

}

bool VillageSystem::left_click() noexcept {
    //return m_ui->check_text_button_list(m_ui->m_toolsmith_buy_buttons);
    return false;
}

void VillageSystem::step(const float delta) noexcept {
    (void) delta;
}




