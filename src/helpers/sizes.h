#pragma once
#include "util.h"
#include "glm/vec2.hpp"
#include "glm/detail/func_packing_simd.inl"
//
// Created by luisb on 25.06.2026.
//


constexpr glm::vec2 base_icon_size = {32, 32};
constexpr glm::vec2 base_helper_size = {300, 600};

constexpr int normal_field_width = 10;
constexpr int normal_filed_height = 15;
constexpr int normal_field_amount = normal_field_width * normal_filed_height;

// Normal Sizes
constexpr glm::vec2 normal_topleft = {0.1, config::CAMERA_VIEW_HEIGHT-0.7};
constexpr glm::vec2 small_button_size = {0.5, 0.25};
constexpr glm::vec2 normal_button_size = {1.0, 0.5};
constexpr glm::vec2 normal_long_button_size = {2.0, 0.5};
constexpr glm::vec2 medium_button_size = {4.0, 0.5};
constexpr float small_gap = 0.05f;
constexpr float normal_gap = 0.1f;

constexpr float small_text_box_height = 0.3f;
constexpr float normal_text_box_height = 0.5f;

constexpr float small_text_box_text_size = 0.002f;
constexpr float max_button_text_size = 0.004f;
constexpr float normal_text_box_size = 0.009f;
constexpr float small_popup_text_size = 0.006f;
constexpr float normal_popup_text_size = 0.008f;

// Mode selection
constexpr glm::vec2 mode_select_buttons_topleft = {0.1, config::CAMERA_VIEW_HEIGHT - 0.1};

// Field
constexpr glm::vec2 field_cell_topleft = {0.1, config::CAMERA_VIEW_HEIGHT-0.65};
constexpr glm::vec2 field_plant_selection_topleft = {6.0, config::CAMERA_VIEW_HEIGHT-1.2};
constexpr glm::vec2 field_helper_topleft = {config::CAMERA_VIEW_WIDTH - medium_button_size.x - normal_gap, field_plant_selection_topleft.y};
constexpr float field_size = 0.5;
constexpr glm::vec2 full_field_size = glm::vec2{field_size*normal_field_width, field_size*normal_filed_height};//
constexpr glm::vec2 full_field_center = glm::vec2{0.25, 0.25 + field_size*normal_filed_height} + glm::vec2{full_field_size.x/2.0f, -full_field_size.y/2.0f};
constexpr glm::vec2 field_tool_cursor_size = full_field_size + glm::vec2{normal_gap, normal_gap};

constexpr glm::vec2 helper_config_topleft = {normal_topleft.x, field_plant_selection_topleft.y};
constexpr glm::vec2 helper_config_size = {config::CAMERA_VIEW_WIDTH-normal_topleft.x*2.0f, field_plant_selection_topleft.y - normal_gap};
constexpr glm::vec2 helper_config_field_offset = {normal_button_size.x + normal_gap, -small_gap};


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

// Achievements
constexpr glm::vec2 achievement_icon_size = {0.7, 0.7};
constexpr int achievement_am_width = 16;

// Gambling
constexpr glm::vec2 chip_size = {0.5, 0.5};
constexpr glm::vec2 chip_row_start = {config::CAMERA_VIEW_WIDTH/2.0f - chip_size.x - normal_gap, normal_button_size.y + normal_gap*2 + chip_size.y/2.0};
constexpr glm::vec2 chip_row_left = {normal_gap + chip_size.x/2.0, normal_gap + chip_size.y/2.0};
constexpr glm::vec2 chip_row_top_right = {config::CAMERA_VIEW_WIDTH - normal_gap, config::CAMERA_VIEW_HEIGHT - normal_gap};
constexpr glm::vec2 chip_path = chip_row_left - chip_row_start;
constexpr glm::vec2 chip_path_two = chip_row_top_right - chip_row_left;

constexpr glm::vec2 card_size = {1.0f, 1.0f};
constexpr glm::vec2 card_deck_pos = {config::CAMERA_VIEW_WIDTH - small_gap - card_size.x/2.0f, config::CAMERA_VIEW_HEIGHT / 2.0f};
constexpr float player_card_hand_y = 3.0f;
constexpr float dealer_card_hand_y = 6.0f;

// Skilltree
constexpr glm::vec2 skill_button_size = {1.0, 1.5};

// Message Popup
constexpr float message_popup_w = 3.0f;
constexpr glm::vec2 message_popup_title_size = {message_popup_w - small_gap*2, normal_text_box_height};
constexpr glm::vec2 message_popup_description_size = {message_popup_w - small_gap*2, small_text_box_height};
constexpr glm::vec2 message_popup_close_button_size = {message_popup_w - small_gap*2, 0.5};
constexpr float message_popup_h = message_popup_title_size.y + message_popup_description_size.y + message_popup_close_button_size.y*2 + small_gap*5;

constexpr glm::vec2 message_popup_size = {message_popup_w, message_popup_h};
constexpr glm::vec2 message_popup_topleft = {config::CAMERA_VIEW_WIDTH - normal_gap - message_popup_size.x, message_popup_size.y + normal_gap};


constexpr glm::vec2 message_popup_title_center = {message_popup_topleft.x + small_gap + message_popup_title_size.x/2.0f, message_popup_topleft.y - small_gap - message_popup_title_size.y/2.0f};
constexpr glm::vec2 message_popup_description_center =
    message_popup_title_center - glm::vec2{0.0f, message_popup_title_size.y/2.0f + small_gap + message_popup_description_size.y/2.0f};
constexpr glm::vec2 message_popup_close_all_button_center =
    message_popup_topleft + glm::vec2{message_popup_size.x / 2.0f, -message_popup_size.y + small_gap + message_popup_close_button_size.y / 2.0f};
constexpr glm::vec2 message_popup_close_button_center =
    message_popup_topleft + glm::vec2{message_popup_size.x / 2.0f, -message_popup_size.y + small_gap + message_popup_close_button_size.y + small_gap + message_popup_close_button_size.y/2.0f};

constexpr glm::vec2 message_popup_center = message_popup_topleft + (glm::vec2{message_popup_size.x, -message_popup_size.y} / 2.0f);
