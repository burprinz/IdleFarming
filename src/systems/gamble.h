#pragma once
#include "window.h"
#include "core/registry.h"
#include "glm/vec2.hpp"
#include "glm/detail/func_packing_simd.inl"
#include "helpers/sizes.h"
//
// Created by luisb on 27.08.2026.
//

class GambleSystem {
    public:
    static GambleSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;

private:

    Registry* m_registry;
    Window* m_window;

    struct CardAnimation {
        entt::entity card;
        glm::vec2 start;
        glm::vec2 end;
    };
    std::vector<CardAnimation> current_card_animations;
    bool current_wave_for_player = false;

    inline std::vector<glm::vec2> compute_hand_positions(int n, float row_y) const noexcept {
        float total_width = n * card_size.x + (n - 1) * small_gap;
        float left_x = config::CAMERA_VIEW_WIDTH / 2.0f - total_width / 2.0f + card_size.x / 2.0f;

        std::vector<glm::vec2> positions;
        positions.reserve(n);
        for (int i = 0; i < n; i++) {
            positions.push_back({left_x + i * (card_size.x + small_gap), row_y});
        }
        return positions;
    }

    inline void copy_card_deck_and_shuffle() {
        card_deck.clear();
        card_deck = m_registry->card_list;
        static std::mt19937 RNG{ std::random_device{}() };
        std::shuffle(card_deck.begin(), card_deck.end(), RNG);
    }

    inline entt::entity draw_card() {
        assert(!card_deck.empty() && "draw_last_card: deck is empty");

        entt::entity card = card_deck.back();
        card_deck.pop_back();
        return card;
    }

    inline void end_round() {
        bool player_natural = m_registry->player_value == 21 && m_registry->player_cards.size() == 2;
        bool dealer_natural = m_registry->dealer_value == 21 && m_registry->dealer_cards.size() == 2;

        if (player_natural && dealer_natural) {
            round_winner = "Push";
        } else if (player_natural) {
            round_winner = "Player";
        } else if (dealer_natural) {
            round_winner = "Dealer";
        } else if (m_registry->player_value > 21) {
            round_winner = "Dealer";
        } else if (m_registry->dealer_value > 21) {
            round_winner = "Player";
        } else if (m_registry->player_value > m_registry->dealer_value) {
            round_winner = "Player";
        } else if (m_registry->dealer_value > m_registry->player_value) {
            round_winner = "Dealer";
        } else {
            round_winner = "Push";
        }

        m_registry->blackjack_action = BLACKJACK_NOTHING;
        m_registry->gamble_stage = BLACKJACK_OVER;
        round_over_printed = false;
        reset_animation();
    }

    inline void reset_animation() {
        animation_done = false;
        animation_decelerating = false;
        animation_pos = 0.0f;
        animation_vel = 0.0f;
    }

    inline bool step_animation_curve(const float delta) {
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

        animation_pos += delta * animation_vel;

        if (animation_pos > 1.0f) {
            animation_pos = 1.0f;
            animation_done = true;
        }

        return animation_done;
    }

    inline void draw_card_animated(std::vector<entt::entity> &hand, float row_y, bool for_player) {
        current_wave_for_player = for_player;

        int old_count = static_cast<int>(hand.size());
        std::vector<glm::vec2> old_positions = compute_hand_positions(old_count, row_y);
        std::vector<glm::vec2> new_positions = compute_hand_positions(old_count + 1, row_y);

        for (int i = 0; i < old_count; i++) {
            current_card_animations.push_back({hand[i], old_positions[i], new_positions[i]});
        }

        entt::entity new_card = draw_card();
        hand.push_back(new_card);

        if (!for_player && hand.size() == 1) {
            m_registry->ecs.get<PlayingCard>(new_card).hidden = true;
        }

        m_registry->ecs.get<Position>(new_card).pos = card_deck_pos;
        current_card_animations.push_back({new_card, card_deck_pos, new_positions[old_count]});

        reset_animation();
    }

    inline void reset_blackjack() {
        m_registry->player_cards.clear();
        m_registry->dealer_cards.clear();
        m_registry->bet_chips.clear();
        m_registry->bet_amount = 0;
        m_registry->blackjack_action = BLACKJACK_NOTHING;
        m_registry->gamble_stage = BLACKJACK_BETTING;
        m_registry->ecs.get<Position>(m_registry->chips).pos = chip_row_start;
        round_winner = "";
        chips_moved = false;
        start_dealer_cards_drawn = 0;
        start_player_cards_drawn = 0;
        pending_blackjack_action = BLACKJACK_NOTHING;
        drawing_for_dealer = false;
        current_wave_for_player = false;
        card_deck.clear();
    }

    std::vector<entt::entity> card_deck;

    bool animation_done = false;
    bool animation_decelerating = false;
    float animation_pos = 0.0f;
    float animation_vel = 0.0f;
    float animation_acc = 33.5f; // 1s = 4.45
    float animation_min_vel = 0.4f; // 1s = 0.3

    // Blackjack
    bool chips_moved = false;
    int start_dealer_cards_drawn = 0;
    int start_player_cards_drawn = 0;
    int pending_blackjack_action = BLACKJACK_NOTHING;
    bool drawing_for_dealer = false;

    std::string round_winner;
    bool round_over_printed = false;

    // Card dealing animation
    entt::entity current_drawn_card = entt::null;
    glm::vec2 current_card_target;
    bool current_card_for_player = false;

    void blackjackStep(const float delta);

};
