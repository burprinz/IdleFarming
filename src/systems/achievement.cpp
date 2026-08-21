//
// Created by luisb on 21.08.2026.
//

#include "achievement.h"

#include "core/stats.h"


AchievementSystem AchievementSystem::init(Window *window, Registry *registry) noexcept {
    AchievementSystem self;
    self.m_registry = registry;
    self.m_window = window;
    return self;
}

void AchievementSystem::deinit() noexcept {

}

void AchievementSystem::reset() noexcept {

}

void AchievementSystem::step(const float delta) noexcept {
    (void) delta;
    for (entt::entity a_id : m_registry->ecs.view<Achievement>()) {
        Achievement &a = m_registry->ecs.get<Achievement>(a_id);
        if (!a.done) {
            BigNumber amount = BigNumber(0);
            for (std::string stat : a.needed_stat_types) {
                amount = amount + StatsManager::instance().get(stat);
            }
            if (amount >= a.amount) {
                a.done = true;
            }
        }
    }
}