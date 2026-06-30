#include "render.h"
#include "config.h"
#include "core/registry.h"

#include <iostream>

#include "core/colors.h"
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
        bool fieldMarked = false;
        glm::vec2 markedPos, markedSize;

        /**
         * Draw Field
         */
        glm::vec3 fieldColor = {0.82, 0.49, 0.18};
        for (entt::entity fieldID : m_registry->ecs.view<FarmField>()) {
            FarmField f = m_registry->ecs.get<FarmField>(fieldID);
            glm::vec2 fPos = m_registry->ecs.get<Position>(fieldID).pos;
            glm::vec2 fSize = m_registry->ecs.get<Dimension>(fieldID).dim;
            m_draw->fillRectBySize(fPos, fSize, fieldColor, 1.0f);

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
                float radius = (f.grow / p.growTime) * (fSize.x / 2) * 0.9;
                TextureInformation icon = m_registry->ecs.get<IconTexture>(f.plant).info;
                m_draw->drawTexture(icon.text, fPos, {radius, radius});

            }
        }

        if (fieldMarked) {
            m_draw->drawRectBySize(markedPos, markedSize, green);
        }

        float x = 5.5;
        float y_start = config::CAMERA_VIEW_HEIGHT-1.2;

        m_draw->drawLine({x, 0.0}, {x, y_start}, black);

        for (TextButton b : m_ui->m_field_plant_select_buttons) {
            entt::entity plant_id = m_registry->plants[b.text];
            Plant p = m_registry->ecs.get<Plant>(plant_id);
            text = p.name;
            m_draw->drawStringInBox(text, b.pos, b.size, black);

            glm::vec3 color;
            if (plant_id == m_registry->current_selected_plant) {
                color = selected;
            } else {
                color = b.selected?b.select_color:b.unselect_color;
            }

            m_draw->drawRectBySize(b.pos, b.size, color);
        }

        // Wenn aktuell eine Pflanze selected ist, gib die infos
        if (m_registry->current_selected_plant != entt::null) {
            glm::vec2 selectInfoSize = {4.0f, 5.0f};
            glm::vec2 topLeft = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x - 0.2, selectInfoSize.y + 0.2};
            glm::vec2 selectInfoPos = {config::CAMERA_VIEW_WIDTH - selectInfoSize.x/2 - 0.2, selectInfoSize.y/2 + 0.2};
            m_draw->drawRectBySize(selectInfoPos, selectInfoSize, black);
            //std::string selected_plant = m_registry->current_selected_plant;
            entt::entity id = m_registry->current_selected_plant;
            Plant plant = m_registry->ecs.get<Plant>(id);

            float text_height = 0.3f;
            x = selectInfoPos.x;
            float y = topLeft.y - text_height/2.0f - 0.05f;
            m_draw->drawStringInBox(plant.name, {x, y}, {selectInfoSize.x, text_height}, black);

            y -= text_height/2.0f+0.05f;
            m_draw->drawLine({topLeft.x, y}, {topLeft.x+selectInfoSize.x, y}, black);

            y -= text_height/2.0f+0.05f;
            BigNumber am = m_registry->ecs.get<Amount>(id).am;
            m_draw->drawStringInBox("x" + am.toString(), {x, y}, {selectInfoSize.x, text_height}, black);
        }
    } else if (m_registry->window() == GENE_LAB) {

    } else if (m_registry->window() == MARKETPLACE) {
        for (std::vector<TextButton> &buts : m_ui->m_market_trade_buttons) {
            entt::entity p_id = m_registry->plants[buts[0].label];
            Plant p = m_registry->ecs.get<Plant>(p_id);

            float y = buts[0].pos.y;
            glm::vec2 pos = {trade_buttons_topleft.x + plant_name_size.x/2.0f, y};
            m_draw->drawRectBySize(pos, plant_name_size, black);
            m_draw->drawStringInBox(p.name, pos, plant_name_size, black);

            pos = {pos.x + plant_name_size.x/2.0f + market_stash_size.x/2.0f + 0.1f, pos.y};
            BigNumber stash = m_registry->ecs.get<Amount>(p_id).am;
            m_draw->drawRectBySize(pos, market_stash_size, black);
            m_draw->drawStringInBox("x" + stash.toString(), pos, market_stash_size, black);

            for (TextButton &b : buts) {
                glm::vec3 color = b.selected?b.select_color:b.unselect_color;
                m_draw->drawRectBySize(b.pos, b.size, color);
                m_draw->drawStringInBox(b.text, b.pos, b.size, black);
            }
        }
    }
}