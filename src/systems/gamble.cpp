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

    std::cout<<"Player)";
    for (entt::entity card: m_registry->player_cards) {
        std::cout<<" "<<m_registry->ecs.get<PlayingCard>(card).name;
    }
    std::cout<<std::endl;
    std::cout<<"Dealer)";
    for (entt::entity card: m_registry->dealer_cards) {
        std::cout<<" "<<m_registry->ecs.get<PlayingCard>(card).name;
    }
    std::cout<<std::endl;

    if (m_registry->gamble_stage == BLACKJACK_BETTING) {
        chips_moved = false;
        reset_animation();
    } else if (m_registry->gamble_stage == BLACKJACK_TAKING) {

        if (!chips_moved) {
            if (step_animation_curve(delta)) {
                // Start with card austeilung
                glm::vec2 &pos = m_registry->ecs.get<Position>(m_registry->chips).pos;
                pos = chip_row_left;
                chips_moved = true;
                start_dealer_cards_drawn = 0;
                start_player_cards_drawn = 0;
                reset_animation();
                copy_card_deck_and_shuffle();
            } else {
                glm::vec2 &pos = m_registry->ecs.get<Position>(m_registry->chips).pos;
                pos = chip_row_start + chip_path * animation_pos;
            }
        } else if (start_dealer_cards_drawn < 2) {

            if (current_card_animations.empty()) {
                bool for_player = start_player_cards_drawn <= start_dealer_cards_drawn;
                current_wave_for_player = for_player;

                std::vector<entt::entity> &hand = for_player ? m_registry->player_cards : m_registry->dealer_cards;
                float row_y = for_player ? player_card_hand_y : dealer_card_hand_y;

                int old_count = static_cast<int>(hand.size());
                std::vector<glm::vec2> old_positions = compute_hand_positions(old_count, row_y);
                std::vector<glm::vec2> new_positions = compute_hand_positions(old_count + 1, row_y);

                for (int i = 0; i < old_count; i++) {
                    current_card_animations.push_back({hand[i], old_positions[i], new_positions[i]});
                }

                entt::entity new_card = draw_card();
                hand.push_back(new_card);
                m_registry->ecs.get<Position>(new_card).pos = card_deck_pos;
                current_card_animations.push_back({new_card, card_deck_pos, new_positions[old_count]});

                reset_animation();
            }

            bool done = step_animation_curve(delta);

            for (CardAnimation &anim : current_card_animations) {
                glm::vec2 &pos = m_registry->ecs.get<Position>(anim.card).pos;
                if (done) {
                    pos = anim.end;
                } else {
                    pos = anim.start + (anim.end - anim.start) * animation_pos;
                }
            }

            if (done) {
                if (current_wave_for_player) {
                    start_player_cards_drawn++;
                } else {
                    start_dealer_cards_drawn++;
                }

                current_card_animations.clear();
                reset_animation();
            }
        }
    }

    (void) delta;
}
