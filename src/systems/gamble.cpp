//
// Created by luisb on 27.08.2026.
//

#include "gamble.h"

GambleSystem GambleSystem::init(Window *window, Registry *registry) noexcept {
    GambleSystem self;
    self.m_registry = registry;
    self.m_window = window;
    return self;
}

void GambleSystem::deinit() noexcept {

}

void GambleSystem::reset() noexcept {

}

void GambleSystem::step(const float delta) noexcept {
    if (m_registry->gamble_mode == BLACKJACK) {
        blackjackStep(delta);
    }
}

void GambleSystem::blackjackStep(const float delta) {
    (void) delta;
}
