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

    Registry *registry = m_registry;


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

    pos.x += size.x + normal_gap;

    TextButton skilltreeButton;
    skilltreeButton.pos = pos;
    skilltreeButton.size = size;
    skilltreeButton.text = "Skilltree";
    skilltreeButton.on_click = [registry] {
        registry->change_window(SKILLTREE);
    };
    m_window_buttons.push_back(skilltreeButton);
}


void UISystem::deinit() noexcept {
}

void UISystem::reset() noexcept {
}

void UISystem::left_click() noexcept {
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
}

void UISystem::step(const float delta) noexcept {
    (void) delta;

    glm::vec2 mousePos = m_window->getMousePosition();
    selection_found = false;

    // Popup Buttons
    if (messagePopupAvailable()) {
        updateMessagePopupButtons();
        selection_found = update_text_button_list(message_popup_close_buttons, mousePos, false);
    }

    // Window switch buttons
    selection_found = update_text_button_list(m_window_buttons, mousePos, false);

    if (m_registry->window() == FIELDS) {
        // Field buttons
        selection_found = update_text_button_list(m_field_mode_buttons, mousePos, false);

        if (m_registry->field_mode == FIELD_FARMING) {
            // Planting / Harvesting
            selection_found = update_text_button_list(m_field_plant_select_buttons[m_registry->current_selected_field],
                                                      mousePos, false);

            selection_found = update_text_button_list(m_field_helper_buttons[m_registry->current_selected_field],
                                                      mousePos, true);
        } else if (m_registry->field_mode == FIELD_HELPER_CONFIGURATION) {
            // Helper Config
            selection_found = update_text_button_list(m_field_plant_select_buttons[m_registry->current_selected_field],
                                                      mousePos, false, helper_config_field_offset);

            selection_found = update_text_button_list(m_field_helper_config_buttons, mousePos, false);
        }

        (void) selection_found;
    } else if (m_registry->window() == MARKETPLACE) {
        selection_found = update_text_button_list(m_market_category_buttons, mousePos, false);

        // Marketplace buy / sell
        for (std::vector<TextButton> &buttons: m_market_trade_buttons) {
            selection_found = update_text_button_list(buttons, mousePos, true);
        }
    } else if (m_registry->window() == TOOLSMITH) {
        m_registry->current_hover_tool = entt::null;

        for (TextButton &b: m_toolsmith_buy_buttons) {
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
        selection_found = update_text_button_list(m_helper_buy_buttons, mousePos, false);
    } else if (m_registry->window() == GAMBLING) {
        selection_found = update_text_button_list(m_gamble_mode_select_buttons, mousePos, false);
        if (m_registry->gamble_mode == BLACKJACK) {
            if (m_registry->gamble_stage == BLACKJACK_BETTING) {
                selection_found = update_text_button_list(m_blackjack_bet_buttons, mousePos, false);
            } else if (m_registry->gamble_stage == BLACKJACK_TAKING) {
                selection_found = update_text_button_list(m_blackjack_play_buttons, mousePos, false);
            } else if (m_registry->gamble_stage == BLACKJACK_END_SCREEN) {
                selection_found = update_text_button_list(m_blackjack_end_buttons, mousePos, false);
            }
        }
    } else if (m_registry->window() == SKILLTREE) {
        selection_found = update_text_button_list(m_skilltree_buttons, mousePos, false, m_registry->skilltree_pos_offset);
    }
}

void UISystem::update_button_states() noexcept {
    glm::vec2 size = trade_button_size;
    glm::vec2 pos = trade_buttons_topleft;
    pos = calc_center(pos, size);

    /**
     * Update Market Trade Buttons
     */
    for (std::vector<TextButton> &buttons: m_market_trade_buttons) {
        entt::entity item_id = buttons[0].id_label;

        bool should_be_visible = m_registry->ecs.get<Unlockable>(item_id).unlocked;

        // If not unlocked OR Not in current category
        if (!should_be_visible || !is_in_list(
                m_registry->ecs.get<ItemCategory>(m_registry->current_market_category).items, item_id)) {
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
