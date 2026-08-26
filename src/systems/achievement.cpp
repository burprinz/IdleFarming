//
// Created by luisb on 21.08.2026.
//

#include "achievement.h"

#include <iostream>

#include "core/stats.h"
#include "helpers/sizes.h"
#include "helpers/util.h"


AchievementSystem AchievementSystem::init(Window *window, Registry *registry, UISystem* m_ui) noexcept {
    AchievementSystem self;
    self.m_registry = registry;
    self.m_window = window;
    self.m_ui = m_ui;
    return self;
}

void AchievementSystem::init_achievement_hover() noexcept {
    glm::vec2 base_pos = normal_topleft;
    glm::vec2 size = achievement_icon_size;
    base_pos = calc_center(base_pos, size);

    int x = 0;
    int y = 0;
    for (entt::entity id : m_registry->achievements_order) {
        glm::vec2 pos = base_pos + glm::vec2(x*(size.x+normal_gap), y*(size.y+normal_gap));

        m_registry->ecs.emplace<Position>(id, pos);
        m_registry->ecs.emplace<Dimension>(id, size);

        x += 1;
        if (x >= achievement_am_width) {
            x = 0;
            y += 1;
        }
    }
}


void AchievementSystem::deinit() noexcept {

}

void AchievementSystem::reset() noexcept {

}

void AchievementSystem::step(const float delta) noexcept {
    (void) delta;
    update_achievements();
}


void AchievementSystem::update_achievements() noexcept {
    for (entt::entity a_id : m_registry->achievements_order) {
        Achievement &a = m_registry->ecs.get<Achievement>(a_id);
        if (!a.done) {
            BigNumber amount = BigNumber(0);
            for (std::string stat : a.needed_stat_types) {
                amount = amount + StatsManager::instance().get(stat);
            }
            if (amount >= a.amount) {
                a.done = true;
                m_ui->addPopupMessage("Achievement unlocked", a.name);
            }
        }
    }
}
