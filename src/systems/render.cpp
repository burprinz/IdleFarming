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
    std::vector<TextButton> wButtons = m_ui->window_buttons();
    for (size_t i = 0; i < wButtons.size(); i++) {
        TextButton but = wButtons[i];
        glm::vec3 color;
        if (std::cmp_equal(i, m_registry->window())) {
            color = selected;
        } else {
            color = but.selected?but.select_color:but.unselect_color;
        }

        m_draw->drawRectBySize(but.pos, but.size, color);
        m_draw->drawStringInBox(but.text, but.pos, but.size, black);
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
    }
}

void RenderSystem::drawField() noexcept {
    /**
     * Draw Field Selection
     */
    drawBasicTextButtons(m_ui->m_field_mode_buttons);

    /**
     * Draw Field
     */
    drawCurrentField(
        m_registry->plant_field_tiles[m_registry->current_selected_field],
        m_registry->field_colors[m_registry->current_selected_field].c,
        m_ui->m_field_plant_select_buttons[m_registry->current_selected_field],
        m_registry->current_selected_plant[m_registry->current_selected_field]
    );
}

void RenderSystem::drawMarketplace() noexcept {
    drawBasicTextButtons(m_ui->m_market_category_buttons);

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
        pos.x += icon_size.x + normal_name_size.x/2.0f + 0.1f;
        m_draw->drawRectBySize(pos, normal_name_size, black);
        m_draw->drawStringInBox(i.display_name, pos, normal_name_size, black);

        // Amount of items
        pos = {pos.x + normal_name_size.x/2.0f + trade_item_stash_size.x/2.0f + 0.1f, pos.y};
        BigNumber stash = m_registry->ecs.get<Amount>(p_id).am;
        m_draw->drawRectBySize(pos, trade_item_stash_size, black);
        m_draw->drawStringInBox("x" + stash.toString(), pos, trade_item_stash_size, black);

        // Buy / Sell
        drawBasicTextButtons(buts);
    }
}

void RenderSystem::drawToolsmith() noexcept {

        // Draw Tool Buttons
        for (TextButton but : m_ui->m_toolsmith_buy_buttons) {
            if (but.visible) {
                entt::entity tool_id = but.id_label;

                TextureInformation texture = m_registry->ecs.get<TextureInformation>(tool_id);
                m_draw->drawTexture(texture.text, but.pos, but.size/2.0f);

                glm::vec3 color = red;
                Unlockable u = m_registry->ecs.get<Unlockable>(tool_id);
                if (u.unlocked) {
                    if (id_in_map(tool_id, m_registry->current_plant_tools) || id_in_map(tool_id, m_registry->current_harvest_tools)|| id_in_map(tool_id, m_registry->current_collect_tools)) {
                        // Unlocked and selected
                        if (but.selected) {
                            color = light_green;
                        } else {
                            color = green;
                        }
                    } else {
                        // Unlocked but not selected
                        if (but.selected) {
                            color = light_blue;
                        } else {
                            color = blue;
                        }
                    }
                } else {
                    if (but.selected) {
                        color = light_red;
                    }
                }
                m_draw->drawRectBySize(but.pos, but.size, color);
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
            glm::vec3 color = b.selected?b.select_color:b.unselect_color;
            m_draw->drawRectBySize(b.pos, b.size, color);
            m_draw->drawStringInBox(b.text, b.pos, b.size, black);

            if (b.selected) {
                FarmHelper helper = m_registry->ecs.get<FarmHelper>(b.id_label);
                Field field = m_registry->ecs.get<Field>(helper.field);
                TextureInformation texture = m_registry->ecs.get<TextureInformation>(b.id_label);

                m_draw->drawTexture(texture.text, helper_image_center, helper_image_size / 2.0f);

                glm::vec2 pos = helper_image_topleft + glm::vec2{helper_image_size.x/2.0f, -(helper_image_size.y + normal_button_size.y/2.0f)};
                glm::vec2 size = {helper_image_size.x, normal_button_size.y};
                std::string text = "Works on " + field.display_name;
                m_draw->drawStringInBox(text, pos, size, {0, 0, 0});

            }
        }
    }

}




void RenderSystem::drawBasicTextButtons(std::vector<TextButton> &buttons) noexcept {
    for (TextButton &b : buttons) {
        if (b.visible) {
            glm::vec3 color = b.selected?b.select_color:b.unselect_color;
            m_draw->drawRectBySize(b.pos, b.size, color);
            m_draw->drawStringInBox(b.text, b.pos, b.size, black);
        }
    }
}

void RenderSystem::drawCurrentField(std::vector<std::vector<entt::entity>> &field_ids, glm::vec3 color,
    std::vector<TextButton> &buttons, entt::entity current_selected) noexcept {

    bool fieldMarked = false;
    glm::vec2 markedPos, markedSize;

    entt::entity field_inv_id = entt::null;

    for (size_t row_id = 0; row_id < field_ids.size(); row_id++) {
        std::vector<entt::entity> row = field_ids[row_id];
        for (size_t cell_id = 0; cell_id < row.size(); cell_id++) {
            entt::entity fieldID = row[cell_id];

            FarmField f = m_registry->ecs.get<FarmField>(fieldID);
            glm::vec2 fPos = m_registry->ecs.get<Position>(fieldID).pos;
            glm::vec2 fSize = m_registry->ecs.get<Dimension>(fieldID).dim;
            m_draw->fillRectBySize(fPos, fSize, color, 1.0f);

            // Save infos for marked cell
            if (f.marked) {
                fieldMarked = true;
                markedPos = fPos;
                markedSize = fSize;
            }
            m_draw->drawRectBySize(fPos, fSize, black);
            if (!f.is_empty) {
                // Draw the plant if cell is not empty
                Plant p = m_registry->ecs.get<Plant>(f.plant);
                float radius = 0.0;

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
                    radius = (fSize.x / 2) * 0.9;
                }


                IconTexture iconText = m_registry->ecs.get<IconTexture>(f.plant);
                TextureInformation icon = iconText.info;
                if (f.is_regrowable_field && !f.harvestable) {
                    icon = iconText.ungrownInfo;
                }
                m_draw->drawTexture(icon.text, fPos, {radius, radius});
            }
        }
    }


    // If mouse is hovering over field
    if (fieldMarked) {
        m_draw->drawRectBySize(markedPos, markedSize, green);
    }

    // Draw Inv popup
    if (field_inv_id != entt::null) {
        Inventory inv = m_registry->ecs.get<Inventory>(field_inv_id);
        std::string text = inv.amount.toString() + " / " + inv.size.toString();
        glm::vec2 popup_size = {1.0, 0.5};
        glm::vec2 popup_pos = getPopupPositionForMouse(popup_size);
        drawPopup(text, popup_pos, popup_size, white);
    }

    // Separator between field and buttons
    float x = 5.5;
    float y_start = config::CAMERA_VIEW_HEIGHT-1.2;
    m_draw->drawLine({x, 0.0}, {x, y_start}, black);

    // Draw Plant selection
    BigNumber &money = m_registry->ecs.get<Amount>(m_registry->money()).am;
    std::string text;
    for (TextButton b : buttons) {
        entt::entity plant_id = b.id_label;
        //Plant p = m_registry->ecs.get<Plant>(plant_id);
        Item i = m_registry->ecs.get<Item>(plant_id);
        Unlockable u = m_registry->ecs.get<Unlockable>(plant_id);

        glm::vec3 draw_color;

        // For popup later
        bool has_enough_money = false;

        if (!u.unlocked) {
            // Not unlocked
            has_enough_money = has_enough(money, u.price);
            drawLockInBlackQuad(b.pos, b.size, has_enough_money);
            draw_color = b.selected ? b.select_color : b.unselect_color;
        } else {
            // Unlocked
            if (plant_id == current_selected) {
                // Unlocked and selected
                draw_color = selected;
            } else {
                // Unlocked but not selected
                draw_color = b.selected ? b.select_color : b.unselect_color;
            }
            text = i.display_name;
            m_draw->drawStringInBox(text, b.pos, b.size, black);
        }
        m_draw->drawRectBySize(b.pos, b.size, draw_color);

        // Draw Price popup if not unlocked and selected
        if (!u.unlocked && b.selected) {
            glm::vec2 popup_size = {1.0, 0.5};
            glm::vec2 popup_pos = getPopupPositionForMouse(popup_size);
            glm::vec3 text_color = has_enough_money?green:red;
            drawPopup(u.price.toString(), popup_pos, popup_size, text_color);
        }
    }


    // Wenn aktuell eine Pflanze selected ist, gib die infos
    if (current_selected != entt::null) {
        glm::vec2 selectInfoSize = {4.0f, 5.0f};
        glm::vec2 topLeft = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x - 0.2, selectInfoSize.y + 0.2};
        glm::vec2 selectInfoPos = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x/2 - 0.2, selectInfoSize.y/2 + 0.2};
        m_draw->drawRectBySize(selectInfoPos, selectInfoSize, black);
        //Plant plant = m_registry->ecs.get<Plant>(current_selected);
        Item i = m_registry->ecs.get<Item>(current_selected);

        float text_height = 0.3f;
        x = selectInfoPos.x;
        float y = topLeft.y - text_height/2.0f - 0.05f;
        m_draw->drawStringInBox(i.display_name, {x, y}, {selectInfoSize.x, text_height}, black);

        y -= text_height/2.0f+0.05f;
        m_draw->drawLine({topLeft.x, y}, {topLeft.x+selectInfoSize.x, y}, black);

        y -= text_height/2.0f+0.05f;
        BigNumber am = m_registry->ecs.get<Amount>(current_selected).am;
        m_draw->drawStringInBox("x" + am.toString(), {x, y}, {selectInfoSize.x, text_height}, black);
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



