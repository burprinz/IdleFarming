//
// Created by luisb on 21.08.2026.
//

#include "achievement.h"


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
}