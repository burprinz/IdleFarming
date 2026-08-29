//
// Created by luisb on 27.08.2026.
//

#include "gamble.h"

#include "glm/vec2.hpp"
#include "glm/detail/func_packing_simd.inl"
#include "helpers/sizes.h"

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

    if (m_registry->gamble_stage == BLACKJACK_BETTING) {
        animation_done = false;
        animation_decelerating = false;
        animation_pos = 0.0f;
        animation_vel = 0.0f;
    } else if (m_registry->gamble_stage == BLACKJACK_TAKING) {

        if (!animation_done) {
            if (!animation_decelerating && animation_pos > 0.45f) {
                animation_decelerating = true;
            }

            if (animation_decelerating) {
                animation_vel -= delta * animation_acc;
                if (animation_vel < animation_min_vel) {
                    animation_vel = animation_min_vel;
                }
            } else {
                animation_vel += delta * animation_acc;
            }

            animation_pos += delta*animation_vel;

            if (animation_pos > 1.0f) {
                animation_pos = 1.0f;
                animation_done = true;
            }

            glm::vec2 &pos = m_registry->ecs.get<Position>(m_registry->chips).pos;
            if (animation_done) {
                pos = chip_row_left;
            } else {
                pos = chip_row_start + chip_path * animation_pos;
            }
        }
    }

    (void) delta;
}
