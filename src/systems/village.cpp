//
// Created by luisb on 02.09.2026.
//

#include "village.h"

#include "helpers/cloud_generation.h"
#include "helpers/sizes.h"

VillageSystem VillageSystem::init(Window *window, Registry *registry, UISystem *ui) noexcept {
    VillageSystem self;
    self.m_window = window;
    self.m_registry = registry;
    self.m_ui = ui;
    return self;
}

void VillageSystem::deinit() noexcept {

}

void VillageSystem::reset() noexcept {
    initButtons();

    // Alte Wolken aufräumen (Entities UND GL-Texturen)
    for (entt::entity v : m_registry->ecs.view<CloudRange>()) {
        CloudRange &r = m_registry->ecs.get<CloudRange>(v);
        for (entt::entity c : r.clouds) {
            if (m_registry->ecs.valid(c)) m_registry->ecs.destroy(c);
        }
        r.clouds.clear();
    }
    if (!m_registry->cloudTextures.empty()) {
        glDeleteTextures(static_cast<GLsizei>(m_registry->cloudTextures.size()),
                         m_registry->cloudTextures.data());
    }
    m_registry->cloudTextures.clear();
    initClouds(m_registry->cloudTextures, 6);

    if (m_registry->cloudTextures.empty()) return;

    static std::mt19937 rng{std::random_device{}()};
    auto rnd = [&](float a, float b) {
        return std::uniform_real_distribution<float>(a, b)(rng);
    };
    std::uniform_int_distribution<size_t> pick(0, m_registry->cloudTextures.size() - 1);

    const glm::vec2 cloud_size{3.0f, 1.5f};
    const glm::vec2 cloud_half = cloud_size * 0.5f;
    const glm::vec2 cell       = cloud_size * 0.35f;
    const glm::vec2 margin     = cloud_size * 0.25f;

    for (entt::entity village_id : m_registry->ecs.view<Village, CloudRange>()) {
        CloudRange &range = m_registry->ecs.get<CloudRange>(village_id);

        for (float y = range.y_min - margin.y; y < range.y_max + margin.y; y += cell.y) {
            for (float x = min_x - margin.x; x < max_x + margin.x; x += cell.x) {

                glm::vec2 pos{ x + rnd(0.f, cell.x), y + rnd(0.f, cell.y) };
                glm::vec2 half = cloud_half * rnd(0.8f, 1.3f);

                entt::entity cloud = m_registry->ecs.create();
                m_registry->ecs.emplace<Position>(cloud, pos);
                m_registry->ecs.emplace<Dimension>(cloud, half);   // halbe Größe
                m_registry->ecs.emplace<Cloud>(cloud, Cloud{ m_registry->cloudTextures[pick(rng)] });

                range.clouds.push_back(cloud);
            }
        }
    }
}

void VillageSystem::initButtons() noexcept {

}

bool VillageSystem::left_click() noexcept {
    //return m_ui->check_text_button_list(m_ui->m_toolsmith_buy_buttons);
    return false;
}

void VillageSystem::step(const float delta) noexcept {
    if (m_registry->window() == VILLAGE) {
        glm::vec2 mouse_pos = m_window->getMousePosition();

        // Movement
        if (m_registry->keys["mouse_right"]) {
            if (!right_click_pressed && point_in_rect(mouse_pos, village_window_center, village_window_size)) {
                right_click_pressed = true;
                origin_mouse = mouse_pos;
                old_offset = m_registry->village_pos_offset;
            }
            if (right_click_pressed) {
                float dy = mouse_pos.y - origin_mouse.y;
                float target = old_offset.y + dy;
                float clamped = std::clamp(target, -MAX_Y_OFFSET, MIN_Y_OFFSET);
                m_registry->village_pos_offset.y = clamped;

                if (clamped != target) {
                    old_offset.y = clamped;
                    origin_mouse.y = mouse_pos.y;
                }
            }
        } else {
            right_click_pressed = false;
        }

        bool hovering = false;
        bool select_found = false;
        entt::entity selected_entity = entt::null;
        for (entt::entity id : m_registry->ecs.view<Village>()) {
            Unlockable &u = m_registry->ecs.get<Unlockable>(id);
            CloudRange &cloud_range = m_registry->ecs.get<CloudRange>(id);

            if (!u.unlocked) {
                const float band_height = cloud_range.y_max - cloud_range.y_min;
                const glm::vec2 band_center{
                    village_window_center.x,
                    (cloud_range.y_min + cloud_range.y_max) * 0.5f + m_registry->village_pos_offset.y
                };
                const glm::vec2 band_size{ village_window_size.x, band_height };

                if (m_registry->keys["mouse_left"]
                    && point_in_rect(mouse_pos, band_center, band_size)) {
                    u.unlocked = true;
                    }
                continue;
            }

            glm::vec2 village_pos = m_registry->ecs.get<Position>(id).pos + m_registry->village_pos_offset;
            glm::vec2 village_size = m_registry->ecs.get<Dimension>(id).dim;

            if (!cloud_range.clouds.empty()) {
                //const float center_x = m_registry->ecs.get<Position>(id).pos.x;

                std::erase_if(cloud_range.clouds, [&](entt::entity cloud) {
                    glm::vec2 &pos = m_registry->ecs.get<Position>(cloud).pos;
                    const glm::vec2 half = m_registry->ecs.get<Dimension>(cloud).dim;

                    const float dir = pos.x < mid_x ? -1.f : 1.f;
                    const float dist = std::abs(pos.x - mid_x);

                    const float speed = 2.0f + 0.6f * dist;
                    pos.x += dir * speed * delta;

                    const bool gone = (dir < 0.f && pos.x + half.x < min_x)
                                   || (dir > 0.f && pos.x - half.x > max_x);
                    if (gone) {
                        m_registry->ecs.destroy(cloud);
                    }
                    return gone;
                });

                continue;
            }

            Village &v = m_registry->ecs.get<Village>(id);

            if (hovering) {
                v.hovering = false;
                break;
            }

            hovering = point_in_rect(mouse_pos, village_pos, village_size);
            v.hovering = hovering;
            float target_intensity = hovering ? 1.0f : 0.0f;
            float lerp_speed = 8.0f;

            m_registry->village_hover_intensity += (target_intensity - m_registry->village_hover_intensity)
                                                * std::clamp(delta * lerp_speed, 0.0f, 1.0f);

            m_registry->village_select_intensity += (1.0f - m_registry->village_select_intensity)
                                                            * std::clamp(delta * lerp_speed, 0.0f, 1.0f);

            if (!select_found && v.hovering && m_registry->keys["mouse_left"]) {
                select_found = true;
                selected_entity = id;
            }
        }

        if (selected_entity != entt::null) {
            for (entt::entity id : m_registry->ecs.view<Village>()) {
                Village &v = m_registry->ecs.get<Village>(id);
                v.selected = selected_entity == id;
            }
        }
    }
}




