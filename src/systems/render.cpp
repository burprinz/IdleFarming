#include "render.h"
#include "config.h"
#include "core/registry.h"

#include <iostream>

#include "core/colors.h"
#include "core/operations.h"
#include "core/sizes.h"
#include "utils/BigNumber.h"
#include "utils/geometry.h"


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

        /**
         * Draw Field Selection
         */
        drawBasicTextButton(m_ui->m_field_mode_buttons);


        /**
         * Draw Field
         */
        if (m_registry->current_selected_field == PLANT_FIELD) {
            glm::vec3 fieldColor = {0.82, 0.49, 0.18};
            drawCurrentField(
                m_registry->plant_field_tiles,
                fieldColor,
                m_ui->m_field_plant_select_buttons,
                m_registry->current_selected_plant);
        } else if (m_registry->current_selected_field == FLOWER_FIELD) {
            glm::vec3 fieldColor = {0.28, 0.43, 0.21};
            drawCurrentField(
                m_registry->flower_field_tiles,
                fieldColor,
                m_ui->m_field_flower_select_buttons,
                m_registry->current_selected_flower);
        } else if (m_registry->current_selected_field == TREE_FIELD) {
            glm::vec3 fieldColor = {0.28, 0.43, 0.21};
            drawCurrentField(
                m_registry->tree_field_tiles,
                fieldColor,
                m_ui->m_field_tree_select_buttons,
                m_registry->current_selected_tree);
        }

    } else if (m_registry->window() == GENE_LAB) {

    } else if (m_registry->window() == MARKETPLACE) {
        for (std::vector<TextButton> &buts : m_ui->m_market_trade_buttons) {
            entt::entity p_id = buts[0].id_label;
            Plant p = m_registry->ecs.get<Plant>(p_id);

            // Plant name
            float y = buts[0].pos.y;
            glm::vec2 pos = {trade_buttons_topleft.x + plant_name_size.x/2.0f, y};
            m_draw->drawRectBySize(pos, plant_name_size, black);
            m_draw->drawStringInBox(p.name, pos, plant_name_size, black);

            // Amount of plants
            pos = {pos.x + plant_name_size.x/2.0f + market_stash_size.x/2.0f + 0.1f, pos.y};
            BigNumber stash = m_registry->ecs.get<Amount>(p_id).am;
            m_draw->drawRectBySize(pos, market_stash_size, black);
            m_draw->drawStringInBox("x" + stash.toString(), pos, market_stash_size, black);

            // Buy / Sell
            drawBasicTextButton(buts);
        }
    }
}

void RenderSystem::drawBasicTextButton(std::vector<TextButton> &buttons) noexcept {
    for (TextButton &b : buttons) {
        glm::vec3 color = b.selected?b.select_color:b.unselect_color;
        m_draw->drawRectBySize(b.pos, b.size, color);
        m_draw->drawStringInBox(b.text, b.pos, b.size, black);
    }
}

void RenderSystem::drawCurrentField(std::vector<entt::entity> &field_ids, glm::vec3 color,
    std::vector<TextButton> &buttons, entt::entity current_selected) noexcept {

    bool fieldMarked = false;
    glm::vec2 markedPos, markedSize;

    for (entt::entity fieldID : field_ids) {
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

            float grow_time = p.growTime;
            if (f.is_tree_field) {
                grow_time += p.regrowTime;
            }
            radius = (f.grow / grow_time) * (fSize.x / 2) * 0.9;

            IconTexture iconText = m_registry->ecs.get<IconTexture>(f.plant);
            TextureInformation icon = iconText.info;
            if (f.is_tree_field && !f.harvestable) {
                icon = iconText.ungrownInfo;
            }
            m_draw->drawTexture(icon.text, fPos, {radius, radius});
        }
    }

    // If mouse is hovering over field
    if (fieldMarked) {
        m_draw->drawRectBySize(markedPos, markedSize, green);
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
        Plant p = m_registry->ecs.get<Plant>(plant_id);
        text = p.name;
        m_draw->drawStringInBox(text, b.pos, b.size, black);

        Unlockable u = m_registry->ecs.get<Unlockable>(plant_id);

        glm::vec3 draw_color;
        if (!u.unlocked) {
            draw_color = has_enough(money, u.price) ? blue : red;
        } else if (plant_id == current_selected) {
            draw_color = selected;
        } else {
            draw_color = b.selected ? b.select_color : b.unselect_color;
        }
        m_draw->drawRectBySize(b.pos, b.size, draw_color);
    }


    // Wenn aktuell eine Pflanze selected ist, gib die infos
    if (current_selected != entt::null) {
        glm::vec2 selectInfoSize = {4.0f, 5.0f};
        glm::vec2 topLeft = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x - 0.2, selectInfoSize.y + 0.2};
        glm::vec2 selectInfoPos = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x/2 - 0.2, selectInfoSize.y/2 + 0.2};
        m_draw->drawRectBySize(selectInfoPos, selectInfoSize, black);
        Plant plant = m_registry->ecs.get<Plant>(current_selected);

        float text_height = 0.3f;
        x = selectInfoPos.x;
        float y = topLeft.y - text_height/2.0f - 0.05f;
        m_draw->drawStringInBox(plant.name, {x, y}, {selectInfoSize.x, text_height}, black);

        y -= text_height/2.0f+0.05f;
        m_draw->drawLine({topLeft.x, y}, {topLeft.x+selectInfoSize.x, y}, black);

        y -= text_height/2.0f+0.05f;
        BigNumber am = m_registry->ecs.get<Amount>(current_selected).am;
        m_draw->drawStringInBox("x" + am.toString(), {x, y}, {selectInfoSize.x, text_height}, black);
    }

}




