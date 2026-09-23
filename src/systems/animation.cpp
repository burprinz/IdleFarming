//
// Created by luisb on 23.09.2026.
//

#include "animation.h"


AnimationSystem AnimationSystem::init(Window *window, Registry *registry) noexcept {
    AnimationSystem self;
    self.m_registry = registry;
    self.m_window = window;

    for (size_t i = 0; i < self.m_registry->field_order.size(); i++) {
        self.m_field_item_move_animations.push_back(std::vector<TextureMoveAnimation>());
    }

    return self;
}

void AnimationSystem::deinit() noexcept {

}

void AnimationSystem::reset() noexcept {
    for (size_t i = 0; i < m_field_item_move_animations.size(); i++) {
        m_field_item_move_animations[i].clear();
    }
    m_field_item_move_animations.clear();
    for (size_t i = 0; i < m_registry->field_order.size(); i++) {
        m_field_item_move_animations.push_back(std::vector<TextureMoveAnimation>());
    }
}

void AnimationSystem::step(const float delta) noexcept {
    (void) delta;

    if (m_registry->window() == FIELDS) {
        for (size_t i = 0; i < m_field_item_move_animations.size(); i++) {
            std::vector<TextureMoveAnimation> &animations = m_field_item_move_animations[i];

            // Clear all not selected fields
            if (m_registry->current_selected_field != i) {
                // Not selected
                animations.clear();
                continue;
            }

            for (size_t j = 0; j < animations.size();) {
                // Selected -> update animations
                TextureMoveAnimation &move = animations[j];

                if (move.is_finished) {
                    animations.erase(animations.begin() + j);
                    continue;
                }

                move.progress += move.speed*delta;
                if (move.progress >= 1.0f) {
                    move.progress = 1.0f;
                    move.is_finished = true;
                }

                j++;
            }
        }
    }
}


