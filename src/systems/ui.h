#pragma once
#include <iostream>

#include "window.h"
#include "../helpers/colors.h"
#include "core/registry.h"
#include "../helpers/util.h"
#include "core/lang.h"
#include "helpers/finder.h"
//
// Created by luisb on 24.06.2026.
//

struct TextButton {
    glm::vec2 pos = {0,0};
    glm::vec2 size = {1,1};
    std::string text = "Button";
    std::string label = "none";
    std::string lang_label = "?";
    entt::entity id_label = entt::null;
    glm::vec3 unselect_color = black;
    glm::vec3 select_color = green;
    glm::vec3 lock_color = green;
    std::function<void()> on_click;
    bool selected = false;
    bool visible = true;
};

struct MessagePopup {
    std::string title = "Title";
    std::string text = "Message";
};


class UISystem {
public:
    UISystem() = default;
    ~UISystem() = default;

    static UISystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;
    void step(const float delta) noexcept;
    void reset() noexcept;

    inline void initButtons() noexcept {
    }

    void left_click() noexcept;

    void initBaseButtons() noexcept;
    inline void updateMessagePopupButtons() noexcept {
        message_popup_close_buttons[1].text = "Close all (" + std::to_string(m_message_popups.size()) + ")";
    }
    std::vector<TextButton> message_popup_close_buttons;

    void initWindowSwitchButtons() noexcept;
    std::vector<TextButton> m_window_buttons;

    std::vector<TextButton> m_field_mode_buttons;
    std::vector<std::vector<TextButton>> m_field_plant_select_buttons;
    std::vector<std::vector<TextButton>> m_field_helper_buttons;
    std::vector<TextButton> m_field_helper_config_buttons;

    std::vector<TextButton> m_market_category_buttons;
    std::vector<std::vector<TextButton>> m_market_trade_buttons;

    std::vector<TextButton> m_toolsmith_buy_buttons;

    std::vector<TextButton> m_helper_buy_buttons;

    std::vector<TextButton> m_gamble_mode_select_buttons;
    std::vector<TextButton> m_blackjack_bet_buttons;
    std::vector<TextButton> m_blackjack_play_buttons;
    std::vector<TextButton> m_blackjack_end_buttons;

    std::vector<TextButton> m_skilltree_buttons;


    inline void apply_lang_on_buttons() {
        apply_lang_on_single_button_lists(message_popup_close_buttons);
        apply_lang_on_single_button_lists(m_window_buttons);
        apply_lang_on_single_button_lists(m_field_mode_buttons);
        apply_lang_on_single_button_lists(m_field_plant_select_buttons);
        apply_lang_on_single_button_lists(m_field_helper_buttons);
        apply_lang_on_single_button_lists(m_field_helper_config_buttons);
        apply_lang_on_single_button_lists(m_market_category_buttons);
        apply_lang_on_single_button_lists(m_market_trade_buttons);
        apply_lang_on_single_button_lists(m_toolsmith_buy_buttons);
        apply_lang_on_single_button_lists(m_helper_buy_buttons);
        apply_lang_on_single_button_lists(m_gamble_mode_select_buttons);
        apply_lang_on_single_button_lists(m_blackjack_bet_buttons);
        apply_lang_on_single_button_lists(m_blackjack_play_buttons);
        apply_lang_on_single_button_lists(m_blackjack_end_buttons);
        apply_lang_on_single_button_lists(m_skilltree_buttons);
    }


    inline void addPopupMessage(std::string title, std::string text) {
        MessagePopup msg;
        msg.title = title;
        msg.text = text;
        m_message_popups.push_back(msg);
    };
    inline bool messagePopupAvailable() {
        return m_message_popups.size() > 0;
    }
    inline void cloaseAllMessagePopups() {
        m_message_popups.clear();
    }
    inline void closeCurrentMessagePopup() {
        m_message_popups.erase(m_message_popups.begin());
    }
    inline MessagePopup* getCurrentMessagePopup() {
        return &m_message_popups[0];
    }

    void update_button_states() noexcept;


    inline bool check_text_button_list(std::vector<TextButton> &buttons) noexcept {
        for (TextButton &b : buttons) {
            if (b.selected && b.visible) {
                b.on_click();
                return true;
            }
        }
        return false;
    }

private:
    Registry* m_registry;
    Window* m_window;

    std::vector<MessagePopup> m_message_popups;

    bool selection_found = false;

    inline void apply_lang_on_single_button_lists(std::vector<TextButton> &list) {
        for (TextButton &b : list) {
            if (b.lang_label != "?") {
                b.text = get_lang_if_in_map(Language::instance().button(), b.lang_label);
            }
        }
    }

    inline void apply_lang_on_single_button_lists(std::vector<std::vector<TextButton>> &list) {
        for (std::vector<TextButton> &b : list) {
            apply_lang_on_single_button_lists(b);
        }
    }

    inline bool update_text_button_list(std::vector<TextButton> &buttons, glm::vec2 mouse_pos, bool need_unlock, glm::vec2 offset={0,0}, float zoom = 1.0f) noexcept {
        (void) zoom;

        for (TextButton &b : buttons) {
            glm::vec2 pos = b.pos * zoom + offset;
            glm::vec2 size = b.size * zoom;

            if (need_unlock) {
                Unlockable u = m_registry->ecs.get<Unlockable>(b.id_label);
                if (!u.unlocked) {
                    b.selected = false;
                    continue;
                }
            }

            if (!selection_found && b.visible && point_in_rect(mouse_pos, pos, size)) {
                b.selected = true;
                selection_found = true;
            } else {
                b.selected = false;
            }
        }
        return selection_found;
    }

};