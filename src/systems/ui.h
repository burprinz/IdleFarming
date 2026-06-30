#pragma once
#include <iostream>

#include "window.h"
#include "core/colors.h"
#include "core/registry.h"
//
// Created by luisb on 24.06.2026.
//

struct TextButton {
    glm::vec2 pos = {0,0};
    glm::vec2 size = {1,1};
    std::string text = "Button";
    std::string label = "none";
    entt::entity id_label = entt::null;
    glm::vec3 unselect_color = blue;
    glm::vec3 select_color = green;
    glm::vec3 lock_color = green;
    std::function<void()> on_click;
    bool selected = false;
    bool color_locked = false;
};


class UISystem {
public:
    UISystem() = default;
    ~UISystem() = default;

    static UISystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;
    void step(const float delta) noexcept;
    void reset() noexcept;

    void onMouseClickCallback(GLFWwindow* window, int button, int action, int mods) noexcept;


    void initWindowSwitchButtons() noexcept;
    void initPlantSelectButton() noexcept;
    void initMarketplaceTradeButtons() noexcept;


    std::vector<TextButton> m_field_plant_select_buttons;
    std::vector<std::vector<TextButton>> m_market_trade_buttons;

    std::vector<TextButton> get_current_text_buttons() noexcept;

    std::vector<TextButton> window_buttons();

private:
    Registry* m_registry;
    Window* m_window;

    std::vector<TextButton> m_window_buttons;

};