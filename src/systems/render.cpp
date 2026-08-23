#include "render.h"
#include "../helpers/config.h"
#include "core/registry.h"

#include <iostream>

#include "../helpers/colors.h"
#include "../helpers/sizes.h"
#include "helpers/finder.h"
#include "helpers/transaction.h"
#include "utils/BigNumber.h"


constexpr int LIGHT_EFFECT_FRAMECOUNT = 32;          // number of frames in light effect texture
constexpr float LIGHT_EFFECT_ANIMATION_SPEED = 0.1f;        // time (in seconds) per frame
constexpr float SHADOW_EXTEND_DISTANCE = 100.0f;  // distance of shadow cast



RenderSystem RenderSystem::init(Window *window, Registry *registry, UISystem* ui, Draw* draw) noexcept {
    RenderSystem self;

    self.m_window = window;
    self.m_registry = registry;
	self.m_draw = draw;
    self.m_ui = ui;

    return self;
}

void RenderSystem::deinit() noexcept {

}

void RenderSystem::reset() noexcept {

}

void RenderSystem::step(const float /*delta*/) noexcept {

    /**
     * Draw Window switch Buttons
     */
    std::vector<TextButton> wButtons = m_ui->m_window_buttons;
    for (size_t i = 0; i < wButtons.size(); i++) {
        TextButton but = wButtons[i];
        glm::vec3 color;
        if (std::cmp_equal(i, m_registry->window())) {
            //color = selected;
            color = button_outer_select_color;
        } else {
            //color = but.selected?but.select_color:but.unselect_color;
            color = but.selected?button_outer_select_color:button_outer_unselect_color;
        }

        //m_draw->drawRectBySize(but.pos, but.size, color);
        //m_draw->drawStringInBox(but.text, but.pos, but.size, black);
        m_draw->fillFadeRectBySize(but.pos, but.size, color, button_inner_color, button_fading_speed);
        m_draw->drawStringInBox(but.text, but.pos, but.size, button_text_color, max_button_text_size);

    }

    /**
     * Draw cash
     */
    BigNumber money = m_registry->ecs.get<Amount>(m_registry->money()).am;
    std::string text = "Money: " + money.toString();
    m_draw->drawStringInBox(text, {config::CAMERA_VIEW_WIDTH-1.0, config::CAMERA_VIEW_HEIGHT-0.25}, {2.0, 0.5}, black);

    if (m_registry->window() == FIELDS) {
        drawField();
    } else if (m_registry->window() == MARKETPLACE) {
        drawMarketplace();
    } else if (m_registry->window() == TOOLSMITH) {
        drawToolsmith();
    } else if (m_registry->window() == HELPERS) {
        drawHelpers();
    } else if (m_registry->window() == ACHIEVEMENTS) {
        drawAchievements();
    }
}

void RenderSystem::drawField() noexcept {

    // Draw Field Selection
    drawBasicTextButtons(m_ui->m_field_mode_buttons, button_text_color, true);

    // Draw Field
    drawCurrentField(
        m_registry->plant_field_tiles[m_registry->current_selected_field],
        m_registry->field_colors[m_registry->current_selected_field].c,
        m_ui->m_field_plant_select_buttons[m_registry->current_selected_field],
        m_registry->current_selected_plant[m_registry->current_selected_field],
        false
    );

    // Draw Helper Button
    entt::entity current_field_id = m_registry->field_order[m_registry->current_selected_field];
    entt::entity helper_id = m_registry->field_helper[current_field_id];
    if (helper_id != entt::null) {

        Unlockable u = m_registry->ecs.get<Unlockable>(helper_id);
        std::vector<TextButton> &buts = m_ui->m_field_helper_buttons[m_registry->current_selected_field];

        for (TextButton &but : buts) {
            if (u.unlocked) {
                drawBasicTextButton(but, button_text_color, false);
            } else {
                drawLockInBlackQuad(but.pos, but.size, false);
            }
        }
    }

    // Draw Selected Plant Info
    entt::entity current_selected = m_registry->current_selected_plant[m_registry->current_selected_field];
    // Wenn aktuell eine Pflanze selected ist, gib die infos
    if (current_selected != entt::null) {
        glm::vec2 selectInfoSize = {4.0f, 5.0f};
        glm::vec2 topLeft = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x - normal_gap, selectInfoSize.y + normal_gap};
        glm::vec2 selectInfoPos = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x/2 - normal_gap, selectInfoSize.y/2 + normal_gap};
        m_draw->drawRectBySize(selectInfoPos, selectInfoSize, black);
        //Plant plant = m_registry->ecs.get<Plant>(current_selected);
        Item i = m_registry->ecs.get<Item>(current_selected);

        float text_height = 0.3f;
        float x = selectInfoPos.x;
        float y = topLeft.y - text_height/2.0f - 0.05f;
        m_draw->drawStringInBox(i.display_name, {x, y}, {selectInfoSize.x, text_height}, black);

        y -= text_height/2.0f+0.05f;
        m_draw->drawLine({topLeft.x, y}, {topLeft.x+selectInfoSize.x, y}, black);

        y -= text_height/2.0f+0.05f;
        BigNumber am = m_registry->ecs.get<Amount>(current_selected).am;
        m_draw->drawStringInBox("x" + am.toString(), {x, y}, {selectInfoSize.x, text_height}, black);
    }

    // Draw Helper Configuration
    if (m_registry->field_mode == FIELD_HELPER_CONFIGURATION) {
        glm::vec2 pos = calc_center(helper_config_topleft, helper_config_size);
        m_draw->fillRectBySize(pos, helper_config_size, black, 0.95f);

        drawBasicTextButtons(m_ui->m_field_helper_config_buttons, white, false);

        // Draw Config Field
        drawCurrentField(
            m_registry->plant_field_tiles[m_registry->current_selected_field],
            m_registry->field_colors[m_registry->current_selected_field].c,
            m_ui->m_field_plant_select_buttons[m_registry->current_selected_field],
            m_registry->current_selected_plant[m_registry->current_selected_field],
            true
        );

        Texture helper_img = m_registry->ecs.get<TextureInformation>(helper_id).text;
        pos = helper_config_topleft + glm::vec2{helper_config_size.x - helper_image_size.x - small_gap , -small_gap};
        pos = calc_center(pos, helper_image_size);
        m_draw->drawTexture(helper_img, pos, helper_image_size/2.0f);

        FarmHelper helper = m_registry->ecs.get<FarmHelper>(helper_id);
        glm::vec2 size = {helper_image_size.x, normal_button_size.y};
        pos += glm::vec2{0, -helper_image_size.y/2.0f - small_gap - size.y/2.0f};
        m_draw->drawRectBySize(pos, size, white);
        std::string text = std::to_string(helper.working_area.size()) + "/" + std::to_string(helper.amount_fields) + " fields";
        m_draw->drawStringInBox(text, pos, size, white);
    }
}

void RenderSystem::drawMarketplace() noexcept {
    drawBasicTextButtons(m_ui->m_market_category_buttons, button_text_color, true);

    for (std::vector<TextButton> &buts : m_ui->m_market_trade_buttons) {
        if (!buts[0].visible) continue;

        entt::entity p_id = buts[0].id_label;
        Item i = m_registry->ecs.get<Item>(p_id);
        IconTexture it = m_registry->ecs.get<IconTexture>(p_id);

        float y = buts[0].pos.y - 0.1;

        // Item Icon
        glm::vec2 icon_size = (glm::vec2{buts[0].size.y, buts[0].size.y} / 2.0f);
        glm::vec2 pos = (0.1f+glm::vec2{trade_buttons_topleft.x + icon_size.x / 2.0f, y});
        m_draw->drawTexture(it.icon.text, pos, icon_size);

        // Item Name
        pos.x += icon_size.x + medium_button_size.x/2.0f + 0.1f;
        m_draw->drawRectBySize(pos, medium_button_size, black);
        m_draw->drawStringInBox(i.display_name, pos, medium_button_size, black);

        // Amount of items
        pos = {pos.x + medium_button_size.x/2.0f + trade_item_stash_size.x/2.0f + 0.1f, pos.y};
        BigNumber stash = m_registry->ecs.get<Amount>(p_id).am;
        m_draw->drawRectBySize(pos, trade_item_stash_size, black);
        m_draw->drawStringInBox("x" + stash.toString(), pos, trade_item_stash_size, black);

        // Buy / Sell
        drawBasicTextButtons(buts, button_text_color, true);
    }
}

void RenderSystem::drawToolsmith() noexcept {

        // Draw Tool Buttons
        for (TextButton but : m_ui->m_toolsmith_buy_buttons) {
            if (but.visible) {
                entt::entity tool_id = but.id_label;

                TextureInformation texture = m_registry->ecs.get<TextureInformation>(tool_id);
                m_draw->drawTexture(texture.text, but.pos, but.size/2.0f);

                Unlockable u = m_registry->ecs.get<Unlockable>(tool_id);
                //glm::vec3 color = red;
                glm::vec3 color = button_outer_unselect_color;
                if (u.unlocked) {
                    if (id_in_map(tool_id, m_registry->current_plant_tools) || id_in_map(tool_id, m_registry->current_harvest_tools)|| id_in_map(tool_id, m_registry->current_collect_tools)) {
                        // Unlocked and selected
                        color = button_outer_select_color;
                    } else {
                        // Unlocked but not selected
                        color = button_outer_activatable_color;
                    }
                } else {
                    if (but.selected) {
                        //color = light_red;
                        color = button_outer_select_color;
                    }
                }
                m_draw->drawFadeRectBySize(but.pos, but.size, color, button_fading_speed);
            }
        }


        // Draw Hover Info
        if (m_registry->current_hover_tool != entt::null) {
            glm::vec2 selectInfoSize = {4.0f, 5.0f};
            glm::vec2 topLeft = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x - 0.2, selectInfoSize.y + 0.2};
            glm::vec2 selectInfoPos = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x/2 - 0.2, selectInfoSize.y/2 + 0.2};
            m_draw->drawRectBySize(selectInfoPos, selectInfoSize, black);

            Item i = m_registry->ecs.get<Item>(m_registry->current_hover_tool);
            Tool t = m_registry->ecs.get<Tool>(m_registry->current_hover_tool);

            float text_height = 0.3f;
            float x = selectInfoPos.x;
            float y = topLeft.y - text_height/2.0f - 0.05f;
            m_draw->drawStringInBox(i.display_name, {x, y}, {selectInfoSize.x, text_height}, black);

            y -= text_height/2.0f+0.05f;
            m_draw->drawLine({topLeft.x, y}, {topLeft.x+selectInfoSize.x, y}, black);

            y -= text_height/2.0f+0.05f;
            std::string text = "Works on " + t.work_area;
            m_draw->drawStringInBox(text, {x, y}, {selectInfoSize.x, text_height}, black);

            y -= text_height+0.05f;
            text = "";
            if (t.usage == PLANTING) {
                text = "Planting ";
            } else if (t.usage == HARVESTING) {
                text = "Harvesting ";
            } else if (t.usage == COLLECTING) {
                text = "Collecting ";
            }
            text += "radius: " + std::to_string(t.radius);
            m_draw->drawStringInBox(text, {x, y}, {selectInfoSize.x, text_height}, black);

            y -= text_height/2.0f+0.05f;
            m_draw->drawLine({topLeft.x, y}, {topLeft.x+selectInfoSize.x, y}, black);

            y -= text_height/2.0f+0.05f;
            m_draw->drawStringInBox("Price", {x, y}, {selectInfoSize.x, text_height}, black);

            y -= text_height/2.0f+0.05f;
            float quarter = topLeft.x+selectInfoSize.x/4.0f;
            m_draw->drawLine({quarter, y}, {(quarter+selectInfoSize.x/2.0f), y}, black);
            y -= text_height/2.0f+0.05f;

            BuyAmount ba = m_registry->ecs.get<BuyAmount>(m_registry->current_hover_tool);
            for (size_t idx = 0; idx < ba.amounts.size(); idx++) {
                Item b_item = m_registry->ecs.get<Item>(ba.item_ids[idx]);
                Amount am = m_registry->ecs.get<Amount>(ba.item_ids[idx]);

                glm::vec3 c = red;
                if (has_enough(am.am, ba.amounts[idx])) {
                    c = green;
                }

                text = "x" + ba.amounts[idx].toString() + " " + b_item.display_name;
                m_draw->drawStringInBox(text, {x, y}, {selectInfoSize.x, text_height}, c);
                y -= text_height+0.05f;
            }
        }
}

void RenderSystem::drawHelpers() noexcept {
    // Draw Helper buy Buttons

    for (TextButton &b : m_ui->m_helper_buy_buttons) {
        if (b.visible) {
            glm::vec3 color = button_outer_unselect_color;
            if (m_registry->ecs.get<Unlockable>(b.id_label).unlocked) {
                color = button_outer_select_color;
            }
            m_draw->fillFadeRectBySize(b.pos, b.size, color, button_inner_color, button_fading_speed);
            m_draw->drawStringInBox(b.text, b.pos, b.size, button_text_color);

            if (b.selected) {
                FarmHelper helper = m_registry->ecs.get<FarmHelper>(b.id_label);
                Field field = m_registry->ecs.get<Field>(helper.field);
                TextureInformation texture = m_registry->ecs.get<TextureInformation>(b.id_label);

                m_draw->drawTexture(texture.text, helper_image_center, helper_image_size / 2.0f);

                glm::vec2 pos = helper_image_topleft + glm::vec2{helper_image_size.x/2.0f, -(helper_image_size.y + normal_button_size.y/2.0f)};
                glm::vec2 size = {helper_image_size.x, normal_button_size.y};
                std::string text = "Works on " + field.display_name;
                m_draw->drawStringInBox(text, pos, size, {0, 0, 0});

                pos.y -= size.y;
                text = "Level " + std::to_string(helper.level) + " (";
                if (!helper.max_reached) {
                    text += std::to_string(helper.amount_fields) + " / " + std::to_string((normal_filed_height*normal_field_width));
                } else {
                    text += "MAX";
                }
                text += + ")";
                size = {helper_image_size.x, small_button_size.y};
                m_draw->drawStringInBox(text, pos, size, black);

                pos.y  -= size.y;

                if (!helper.max_reached) {
                    Unlockable &u = m_registry->ecs.get<Unlockable>(b.id_label);
                    text = "Buy price";
                    if (u.unlocked) {
                        text = "Upgrade price";
                    }size = {helper_image_size.x, small_button_size.y};
                    m_draw->drawStringInBox(text, pos, size, black);

                    pos.y -= size.y + small_gap;
                    float box_h = pos.y;
                    size = {helper_image_size.x, box_h};
                    pos.y = box_h / 2.0f + small_gap;
                    m_draw->drawRectBySize(pos, size, black);

                    BuyAmount am = m_registry->ecs.get<BuyAmount>(b.id_label);
                    am = add_multiplicator(am, calc_double_multiplicator(1, helper.level));
                    std::vector<std::string> prices;
                    for (size_t i = 0; i < am.item_ids.size(); i++) {
                        prices.push_back(am.amounts[i].toString() + "x " + m_registry->ecs.get<Item>(am.item_ids[i]).display_name);
                    }
                    m_draw->drawLinesInBox(prices, pos, size, black, normal_text_box_size);
                }
            }
        }
    }

}

void RenderSystem::drawAchievements() noexcept {
    glm::vec2 mousePos = m_window->getMousePosition();

    bool found = false;

    for (entt::entity id : m_registry->ecs.view<Achievement>()) {
        Achievement a = m_registry->ecs.get<Achievement>(id);
        glm::vec2 pos = m_registry->ecs.get<Position>(id).pos;
        glm::vec2 size = m_registry->ecs.get<Dimension>(id).dim;
        glm::vec3 color = red;
        if (a.done) {
            color = green;
        }
        m_draw->drawRectBySize(pos, size, color);
        m_draw->drawStringInBox(a.name, pos, size, color);

        if (!found && point_in_rect(mousePos, pos, size)) {
            found = true;
            std::string text = a.description;
            glm::vec2 popup_size = {3.0, 0.5};
            glm::vec2 popup_pos = getPopupPositionForMouse(popup_size);
            drawPopup(text, popup_pos, popup_size, white);
        }

    }
}



void RenderSystem::drawBasicTextButtons(std::vector<TextButton> &buttons, glm::vec3 text_color, bool with_max_size) noexcept {
    for (TextButton &b : buttons) {
        drawBasicTextButton(b, text_color, with_max_size);
    }
}

void RenderSystem::drawBasicTextButton(TextButton &b, glm::vec3 text_color, bool with_max_size) noexcept {
    if (b.visible) {
        glm::vec3 color = b.selected?button_outer_select_color:button_outer_unselect_color;
        m_draw->drawButton(b.text, b.pos, b.size, color, button_inner_color, text_color, with_max_size);
    }
}


void RenderSystem::drawCurrentField(std::vector<std::vector<entt::entity>> &field_ids, glm::vec3 color,
    std::vector<TextButton> &buttons, entt::entity current_selected, bool for_helper) noexcept {

    bool fieldMarked = false;
    glm::vec2 markedPos, markedSize;
    entt::entity field_inv_id = entt::null;

    std::vector<glm::vec2> locked_pos, locked_size;

    for (size_t row_id = 0; row_id < field_ids.size(); row_id++) {
        std::vector<entt::entity> row = field_ids[row_id];
        for (size_t cell_id = 0; cell_id < row.size(); cell_id++) {
            entt::entity fieldID = row[cell_id];

            FarmField f = m_registry->ecs.get<FarmField>(fieldID);
            glm::vec2 fPos = m_registry->ecs.get<Position>(fieldID).pos;
            if (for_helper) {
                fPos += helper_config_field_offset;
            }
            glm::vec2 fSize = m_registry->ecs.get<Dimension>(fieldID).dim;
            m_draw->fillRectBySize(fPos, fSize, color, 1.0f);

            // Save infos for marked cell
            if (f.marked && !f.helper_active) {
                fieldMarked = true;
                markedPos = fPos;
                markedSize = fSize;
            }

            if (f.helper_active) {
                locked_pos.push_back(fPos);
                locked_size.push_back(fSize);
            }

            m_draw->drawRectBySize(fPos, fSize, black);
            if (!f.is_empty) {

                // Draw the plant if cell is not empty
                Plant p = m_registry->ecs.get<Plant>(f.plant);
                float radius = 0.0;

                if (!for_helper) {
                    if (!p.has_inventory) {
                        float grow_time = p.growTime;
                        if (f.is_regrowable_field) {
                            grow_time += p.regrowTime;
                        }
                        radius = (f.grow / grow_time) * (fSize.x / 2) * 0.9;
                    } else {
                        // Has inventory -> direkt full size
                        if (point_in_rect(m_window->getMousePosition(), fPos, fSize)) {
                            field_inv_id = fieldID;
                        }
                        radius = (fSize.x / 2.0) * 0.9;
                    }
                } else {
                    radius = (fSize.x / 2.0f) * 0.9;
                }

                if (!for_helper || f.helper_active) {
                    IconTexture iconText = m_registry->ecs.get<IconTexture>(f.plant);
                    TextureInformation icon = iconText.info;
                    if (f.is_regrowable_field && !f.harvestable) {
                        icon = iconText.ungrownInfo;
                    }
                    m_draw->drawTexture(icon.text, fPos, {radius, radius});
                }
            }
        }
    }


    // If mouse is hovering over field
    if (fieldMarked) {
        m_draw->drawRectBySize(markedPos, markedSize, green);
    }

    for (size_t i = 0; i < locked_pos.size(); i++) {
        m_draw->drawRectBySize(locked_pos[i], locked_size[i], red);
    }


    if (!for_helper) {
        // Draw Inv popup
        if (field_inv_id != entt::null) {
            Inventory inv = m_registry->ecs.get<Inventory>(field_inv_id);
            std::string text = inv.amount.toString() + " / " + inv.size.toString();
            glm::vec2 popup_size = {1.0, 0.5};
            glm::vec2 popup_pos = getPopupPositionForMouse(popup_size);
            drawPopup(text, popup_pos, popup_size, white);
        }
    }

    glm::vec3 normal_color = black;
    if (for_helper) {
        normal_color = white;
    }

    // Separator between field and buttons
    float x = 5.5;
    if (for_helper) {
        x += helper_config_field_offset.x;
    }
    float y_start = config::CAMERA_VIEW_HEIGHT-1.2;
    m_draw->drawLine({x, 0.0}, {x, y_start}, normal_color);


    // Draw Plant selection
    BigNumber &money = m_registry->ecs.get<Amount>(m_registry->money()).am;
    std::string text;
    for (TextButton b : buttons) {
        glm::vec2 but_pos = b.pos;
        glm::vec3 unselect_color = button_outer_unselect_color;
        if (for_helper) {
            but_pos += helper_config_field_offset;
            unselect_color = white;

        }

        entt::entity plant_id = b.id_label;
        Item i = m_registry->ecs.get<Item>(plant_id);
        Unlockable u = m_registry->ecs.get<Unlockable>(plant_id);

        glm::vec3 draw_color;

        // For popup later
        bool has_enough_money = false;


        if (!for_helper) {
            if (!u.unlocked) {
                // Not unlocked
                if (!for_helper) {
                    has_enough_money = has_enough(money, u.price);
                    drawLockInBlackQuad(b.pos, b.size, has_enough_money);
                    draw_color = b.selected ? button_outer_select_color : button_outer_unselect_color;
                }
            } else {
                // Unlocked
                if (plant_id == current_selected) {
                    // Unlocked and selected
                    draw_color = button_outer_select_color;
                } else {
                    // Unlocked but not selected

                    draw_color = b.selected ? button_outer_select_color : unselect_color;
                }
                text = i.display_name;
                m_draw->fillFadeRectBySize(but_pos, b.size, draw_color, button_inner_color, button_fading_speed);
                m_draw->drawStringInBox(text, but_pos, b.size, button_text_color);
            }
        } else {
            if (u.unlocked) {
                // For helper only if unlocked
                draw_color = b.selected||plant_id == current_selected ? button_outer_select_color : unselect_color;
                text = i.display_name;
                m_draw->fillFadeRectBySize(but_pos, b.size, draw_color, button_inner_color, button_fading_speed);
                m_draw->drawStringInBox(text, but_pos, b.size, normal_color);
            }
        }




        // Draw Price popup if not unlocked and selected
        if (!for_helper && !u.unlocked && b.selected) {
            glm::vec2 popup_size = {1.0, 0.5};
            glm::vec2 popup_pos = getPopupPositionForMouse(popup_size);
            glm::vec3 text_color = has_enough_money?green:red;
            drawPopup(u.price.toString(), popup_pos, popup_size, text_color);
        }
    }
}

glm::vec2 RenderSystem::getPopupPositionForMouse(glm::vec2 popup_size) noexcept {
    glm::vec2 mouse_pos = m_window->getMousePosition();

    glm::vec2 result = {0, 0};

    if (mouse_pos.x + popup_size.x > config::CAMERA_VIEW_WIDTH) {
        result.x = mouse_pos.x - popup_size.x / 2.0f;
    } else {
        result.x = mouse_pos.x + popup_size.x / 2.0f;
    }

    if (mouse_pos.y + popup_size.y > config::CAMERA_VIEW_HEIGHT) {
        result.y = mouse_pos.y - popup_size.y / 2.0f;
    } else {
        result.y = mouse_pos.y + popup_size.y / 2.0f;
    }

    return result;
}

void RenderSystem::drawPopup(std::string msg, glm::vec2 pos, glm::vec2 size, glm::vec3 text_color) noexcept {
    float angle = 10.0;

    m_draw->fillRoundRect(pos, size, angle, black, 0.75f);
    m_draw->drawStringInBox(msg, pos, size, text_color);
    m_draw->drawRoundRect(pos, size, angle, black);

}
