#pragma once
#include <iostream>

#include "window.h"
#include "../helpers/colors.h"
#include "core/registry.h"
#include "../helpers/util.h"
//
// Created by luisb on 24.06.2026.
//

struct TextButton {
    glm::vec2 pos = {0,0};
    glm::vec2 size = {1,1};
    std::string text = "Button";
    std::string label = "none";
    entt::entity id_label = entt::null;
    glm::vec3 unselect_color = black;
    glm::vec3 select_color = green;
    glm::vec3 lock_color = green;
    std::function<void()> on_click;
    bool selected = false;
    bool color_locked = false;
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

    inline bool update_text_button_list(std::vector<TextButton> &buttons, glm::vec2 mouse_pos, bool need_unlock, glm::vec2 offset={0,0}) noexcept {
        for (TextButton &b : buttons) {
            if (need_unlock) {
                Unlockable u = m_registry->ecs.get<Unlockable>(b.id_label);
                if (!u.unlocked) {
                    b.selected = false;
                    continue;
                }
            }

            if (!selection_found && b.visible && point_in_rect(mouse_pos, b.pos+offset, b.size)) {
                b.selected = true;
                selection_found = true;
            } else {
                b.selected = false;
            }
        }
        return selection_found;
    }

};