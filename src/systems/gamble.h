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
    std::vector<entt::entity> card_deck;

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

    // Card dealing animation
    entt::entity current_drawn_card = entt::null;
    glm::vec2 current_card_target;
    bool current_card_for_player = false;

    void blackjackStep(const float delta);

};
