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
        chips_moved = false;
        round_over_printed = false;
        m_registry->player_cards.clear();
        m_registry->dealer_cards.clear();
        m_registry->player_value = 0;
        m_registry->dealer_value = 0;
        reset_animation();
    } else if (m_registry->gamble_stage == BLACKJACK_TAKING) {

        m_registry->can_click = false;

        if (!chips_moved) {
            if (step_animation_curve(delta)) {
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
                std::vector<entt::entity> &hand = for_player ? m_registry->player_cards : m_registry->dealer_cards;
                float row_y = for_player ? player_card_hand_y : dealer_card_hand_y;

                draw_card_animated(hand, row_y, for_player);
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

                if (start_dealer_cards_drawn == 2) {
                    // Startkarten komplett -> Werte berechnen und auf Naturals pruefen
                    m_registry->player_value = calculate_blackjack_hand(m_registry, m_registry->player_cards);
                    m_registry->dealer_value = calculate_blackjack_hand(m_registry, m_registry->dealer_cards);

                    bool player_blackjack = m_registry->player_value == 21;
                    bool dealer_blackjack = m_registry->dealer_value == 21;

                    if (player_blackjack || dealer_blackjack) {
                        end_round();
                    }
                }
            }

        } else if (!current_card_animations.empty()) {
            // Eine einzelne Hit-, Double-Down- oder Dealer-Karte fliegt noch rein
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
                current_card_animations.clear();
                reset_animation();

                if (drawing_for_dealer) {
                    drawing_for_dealer = false;
                    m_registry->dealer_value = calculate_blackjack_hand(m_registry, m_registry->dealer_cards);

                    if (m_registry->dealer_value > 21) {
                        end_round(); // Dealer bust -> Player gewinnt
                    }
                    // sonst: naechster Frame prueft erneut, ob Dealer weiterziehen muss
                } else {
                    m_registry->player_value = calculate_blackjack_hand(m_registry, m_registry->player_cards);

                    if (m_registry->player_value > 21) {
                        end_round(); // Player bust -> Dealer gewinnt
                    } else if (pending_blackjack_action == BLACKJACK_DOUBLE_DOWN) {
                        m_registry->blackjack_action = BLACKJACK_STAND;
                    } else if (m_registry->player_value == 21) {
                        // Kein Grund mehr, weiterzuziehen
                        m_registry->blackjack_action = BLACKJACK_STAND;
                    } else {
                        m_registry->blackjack_action = BLACKJACK_NOTHING;
                    }
                    pending_blackjack_action = BLACKJACK_NOTHING;
                }
            }

        } else {
            // Betting Done; Player and Dealer got 2 cards, keine Animation aktiv
            if (m_registry->blackjack_action == BLACKJACK_HIT || m_registry->blackjack_action == BLACKJACK_DOUBLE_DOWN) {

                pending_blackjack_action = m_registry->blackjack_action;
                draw_card_animated(m_registry->player_cards, player_card_hand_y, true);

            } else if (m_registry->blackjack_action == BLACKJACK_STAND) {

                if (m_registry->dealer_value < 17) {
                    drawing_for_dealer = true;
                    draw_card_animated(m_registry->dealer_cards, dealer_card_hand_y, false);
                } else {
                    end_round();
                }

            } else {
                m_registry->can_click = true;
            }
        }
    } else if (m_registry->gamble_stage == BLACKJACK_OVER) {

        m_registry->can_click = true;

        BigNumber amount = BigNumber(m_registry->bet_amount);
        BigNumber &am = m_registry->ecs.get<Amount>(m_registry->money()).am;
        if (round_winner == "Player") {
            am += amount;
        } else if (round_winner == "Dealer") {
            am = am - amount;
        }

        std::cout << round_winner << std::endl;
        reset_blackjack();

        // TODO: Win/Lose screen am ende; Erste Dealer Karte am Anfang von Stand aufdecken


    } else {
        m_registry->can_click = true;
    }

    (void) delta;
}