#pragma once
//
// Created by luisb on 25.06.2026.
//


constexpr glm::vec2 base_icon_size = {32, 32};
constexpr glm::vec2 base_helper_size = {300, 600};


// Normal Sizes
constexpr glm::vec2 normal_topleft = {0.1, config::CAMERA_VIEW_HEIGHT-0.7};
constexpr glm::vec2 normal_button_size = {1.0, 0.5};
constexpr glm::vec2 medium_button_size = {4.0, 0.5};
constexpr float normal_gap = 0.1f;

// Mode selection
constexpr glm::vec2 mode_select_buttons_topleft = {0.1, config::CAMERA_VIEW_HEIGHT - 0.1};

// Field
constexpr glm::vec2 field_cell_topleft = {0.1, config::CAMERA_VIEW_HEIGHT-0.65};
constexpr glm::vec2 field_plant_selection = {6.0, config::CAMERA_VIEW_HEIGHT-1.2};
constexpr glm::vec2 field_helper_topleft = {config::CAMERA_VIEW_WIDTH - medium_button_size.x - normal_gap, field_plant_selection.y};
constexpr float field_size = 0.5;

// Market
constexpr glm::vec2 trade_buttons_topleft = {0.1, config::CAMERA_VIEW_HEIGHT-1.3};
constexpr glm::vec2 trade_button_size = {0.7, 0.5};
constexpr glm::vec2 trade_item_stash_size = {2.0, 0.5};

// Toolsmith
constexpr glm::vec2 tool_item_buy_size = {0.5, 0.5};

// Helpers
constexpr glm::vec2 helper_image_size = {3.0, 6.0};
constexpr glm::vec2 helper_image_topleft = glm::vec2{config::CAMERA_VIEW_WIDTH - normal_gap - helper_image_size.x, normal_topleft.y};
constexpr glm::vec2 helper_image_center = helper_image_topleft + glm::vec2{helper_image_size.x, 0} - helper_image_size/2.0f;
