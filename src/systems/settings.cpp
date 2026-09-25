//
// Created by luisb on 25.09.2026.
//

#include "settings.h"

//
// Created by luisb on 02.09.2026.
//

#include "village.h"
#include "core/settings_manager.h"

#include "helpers/sizes.h"

SettingsSystem SettingsSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    SettingsSystem self;
    self.m_window = window;
    self.m_registry = registry;
    self.m_ui = ui;
    return self;
}

void SettingsSystem::deinit() noexcept {

}

void SettingsSystem::reset() noexcept {
    initButtons();
}

void SettingsSystem::initButtons() noexcept {
    m_ui->m_settings_buttons.clear();

    UISystem* ui = m_ui;

    glm::vec2 top_left = field_cell_topleft;
    glm::vec2 size = normal_button_size + glm::vec2{1.0, 0.0};
    glm::vec2 center = calc_center(top_left, size);

    size_t idx = m_ui->m_settings_buttons.size();
    TextButton field_harvest_animation_button;
    field_harvest_animation_button.text = "Harvest Animation (ON)";
    field_harvest_animation_button.pos = center;
    field_harvest_animation_button.size = size;
    field_harvest_animation_button.on_click = [ui, idx] {
        SettingsManager::instance().farmed_plants_animation_enabled = !SettingsManager::instance().farmed_plants_animation_enabled;
        std::string state = "(OFF)";
        if (SettingsManager::instance().farmed_plants_animation_enabled) {
            state = "(ON)";
        }
        ui->m_settings_buttons[idx].text = "Harvest Animation " + state;
    };
    m_ui->m_settings_buttons.push_back(field_harvest_animation_button);
}

bool SettingsSystem::left_click() noexcept {
    return m_ui->check_text_button_list(m_ui->m_settings_buttons);
}

void SettingsSystem::step(const float delta) noexcept {
    (void) delta;


}




