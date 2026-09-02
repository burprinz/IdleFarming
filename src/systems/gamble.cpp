//
// Created by luisb on 27.08.2026.
//

#include "gamble.h"

#include "glm/vec2.hpp"
#include "glm/detail/func_packing_simd.inl"
#include "helpers/sizes.h"

GambleSystem GambleSystem::init(Window *window, Registry *registry, UISystem* ui) noexcept {
    GambleSystem self;
    self.m_registry = registry;
    self.m_window = window;
    self.m_ui = ui;
    return self;
}

void GambleSystem::deinit() noexcept {
}

void GambleSystem::reset() noexcept {
    initButtons();
}

void GambleSystem::initButtons() noexcept {
    m_ui->m_gamble_mode_select_buttons.clear();
    m_ui->m_blackjack_bet_buttons.clear();
    m_ui->m_blackjack_play_buttons.clear();
    m_ui->m_blackjack_end_buttons.clear();

    Registry *registry = m_registry;
    UISystem *ui = m_ui;

    glm::vec2 top_left = field_cell_topleft;
    glm::vec2 size = normal_button_size;
    glm::vec2 center = calc_center(top_left, size);

    TextButton blackjackB;
    blackjackB.text = "Blackjack";
    blackjackB.pos = center;
    blackjackB.size = size;
    blackjackB.on_click = [registry] {
        if (registry->gamble_mode != BLACKJACK) {
            registry->ecs.get<Position>(registry->chips).pos = chip_row_start;
            registry->gamble_stage = BLACKJACK_BETTING;
            registry->can_click = true;
        }
        registry->gamble_mode = BLACKJACK;
    };
    m_ui->m_gamble_mode_select_buttons.push_back(blackjackB);

    size = normal_long_button_size;
    top_left = {normal_gap, normal_gap + normal_button_size.y};
    center = calc_center(top_left, size);

    size_t idx = m_ui->m_blackjack_bet_buttons.size();
    TextButton multiplicatorB;
    multiplicatorB.text = "Bet Amount (1)";
    multiplicatorB.pos = center;
    multiplicatorB.size = size;
    multiplicatorB.on_click = [registry, ui, idx] {
        int current_mult = registry->bet_multiplicator;
        int next_mult = current_mult * 10;
        if (next_mult > 1000) {
            next_mult = 1;
        }
        registry->bet_multiplicator = next_mult;
        ui->m_blackjack_bet_buttons[idx].text = "Bet Amount (" + std::to_string(registry->bet_multiplicator) + ")";
    };
    m_registry->bet_multiplicator = 1;
    m_ui->m_blackjack_bet_buttons.push_back(multiplicatorB);

    float x_middle = config::CAMERA_VIEW_WIDTH / 2.0f;
    float y_middle = config::CAMERA_VIEW_HEIGHT / 2.0f;

    size = normal_button_size;
    center = {config::CAMERA_VIEW_WIDTH / 2.0f, normal_button_size.y / 2.0 + normal_gap};
    TextButton placeBet;
    placeBet.text = "Place Bet";
    placeBet.pos = center;
    placeBet.size = size;
    placeBet.on_click = [registry] {
        if (registry->bet_amount > 0 && BigNumber(registry->bet_amount) <= registry->ecs.get<Amount>(registry->money()).
            am) {
            registry->ecs.get<Position>(registry->chips).pos = chip_row_start;
            registry->gamble_stage = BLACKJACK_TAKING;
            registry->can_click = false;
        }
    };
    m_ui->m_blackjack_bet_buttons.push_back(placeBet);

    top_left = {x_middle - normal_button_size.x * 1.5f - normal_gap, normal_gap + normal_button_size.y};
    size = normal_button_size;
    center = calc_center(top_left, size);

    TextButton betLower;
    betLower.text = "Lower";
    betLower.pos = center;
    betLower.size = size;
    betLower.on_click = [registry] {
        registry->bet_amount -= registry->bet_multiplicator;
        if (registry->bet_amount < 0) {
            registry->bet_amount = 0;
        }
        registry->bet_chips = create_chip_list(registry, registry->bet_amount);
    };
    m_ui->m_blackjack_bet_buttons.push_back(betLower);

    top_left = {x_middle + normal_button_size.x * 0.5f + normal_gap, normal_gap + normal_button_size.y};
    size = normal_button_size;
    center = calc_center(top_left, size);

    TextButton betHigher;
    betHigher.text = "Higher";
    betHigher.pos = center;
    betHigher.size = size;
    betHigher.on_click = [registry] {
        registry->bet_amount += registry->bet_multiplicator;
        if (registry->bet_amount > registry->max_bet_amount) {
            registry->bet_amount = registry->max_bet_amount;
        }
        registry->bet_chips = create_chip_list(registry, registry->bet_amount);
    };
    m_ui->m_blackjack_bet_buttons.push_back(betHigher);

    size = normal_button_size;
    center = {config::CAMERA_VIEW_WIDTH / 2.0f, normal_button_size.y / 2.0 + normal_gap};
    TextButton double_down;
    double_down.text = "Double Down";
    double_down.pos = center;
    double_down.size = size;
    double_down.on_click = [registry] {
        int doubled_bet = registry->bet_amount * 2;
        if (registry->player_cards.size() == 2 && BigNumber(doubled_bet) <= registry->ecs.get<Amount>(registry->money())
            .am) {
            registry->bet_amount = doubled_bet;
            registry->bet_chips = create_chip_list(registry, registry->bet_amount);
            registry->blackjack_action = BLACKJACK_DOUBLE_DOWN;
        }
    };
    m_ui->m_blackjack_play_buttons.push_back(double_down);

    top_left = {x_middle - normal_button_size.x * 1.5f - normal_gap, normal_gap + normal_button_size.y};
    size = normal_button_size;
    center = calc_center(top_left, size);
    TextButton hit;
    hit.text = "Hit";
    hit.pos = center;
    hit.size = size;
    hit.on_click = [registry] {
        registry->blackjack_action = BLACKJACK_HIT;
    };
    m_ui->m_blackjack_play_buttons.push_back(hit);


    top_left = {x_middle + normal_button_size.x * 0.5f + normal_gap, normal_gap + normal_button_size.y};
    size = normal_button_size;
    center = calc_center(top_left, size);
    TextButton stand;
    stand.text = "Stand";
    stand.pos = center;
    stand.size = size;
    stand.on_click = [registry] {
        registry->blackjack_action = BLACKJACK_STAND;
    };
    m_ui->m_blackjack_play_buttons.push_back(stand);

    size = normal_button_size;
    center = {x_middle, y_middle - small_gap - size.y / 2.0f};
    TextButton endGame;
    endGame.text = "End Round";
    endGame.pos = center;
    endGame.size = size;
    endGame.on_click = [registry] {
        registry->gamble_stage = BLACKJACK_OVER;
    };
    m_ui->m_blackjack_end_buttons.push_back(endGame);
}

bool GambleSystem::left_click() noexcept {
    if (m_ui->check_text_button_list(m_ui->m_gamble_mode_select_buttons)) {
        return true;
    }

    if (m_registry->gamble_mode == BLACKJACK) {
        if (m_registry->gamble_stage == BLACKJACK_BETTING) {
            if (m_ui->check_text_button_list(m_ui->m_blackjack_bet_buttons)) {
                return true;
            }
        } else if (m_registry->gamble_stage == BLACKJACK_TAKING) {
            if (m_ui->check_text_button_list(m_ui->m_blackjack_play_buttons)) {
                return true;
            }
        } else if (m_registry->gamble_stage == BLACKJACK_END_SCREEN) {
            if (m_ui->check_text_button_list(m_ui->m_blackjack_end_buttons)) {
                return true;
            }
        }
    }

    return false;
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
            // 1. Move Chips to Bot left
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
            // 2. Draw 2 cards for dealer and player
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
                    std::cout<<"Blackjack Winner) "<<round_winner<<std::endl;
                }

            } else {
                m_registry->can_click = true;
            }
        }
    } else if (m_registry->gamble_stage == BLACKJACK_END_SCREEN) {

        m_registry->can_click = true;
        m_registry->winner_name = round_winner;
        entt::entity first_dealer_card_id = m_registry->dealer_cards[0];
        PlayingCard &card = m_registry->ecs.get<PlayingCard>(first_dealer_card_id);
        card.hidden = false;
        reset_animation();

        // TODO: Win/Lose screen am ende; Erste Dealer Karte am Anfang von Stand aufdecken


    } else if (m_registry->gamble_stage == BLACKJACK_OVER) {


        if (round_winner == "Player") {
            if (step_animation_curve(delta)) {
                glm::vec2 &pos = m_registry->ecs.get<Position>(m_registry->chips).pos;
                pos = chip_row_top_right;
                BigNumber amount = BigNumber(m_registry->bet_amount);
                BigNumber &am = m_registry->ecs.get<Amount>(m_registry->money()).am;
                am += amount;
                reset_animation();
                reset_blackjack();
            } else {
                glm::vec2 &pos = m_registry->ecs.get<Position>(m_registry->chips).pos;
                pos = chip_row_left + chip_path_two * animation_pos;
            }
        } else {
            if (round_winner == "Dealer") {
                BigNumber amount = BigNumber(m_registry->bet_amount);
                BigNumber &am = m_registry->ecs.get<Amount>(m_registry->money()).am;
                am = am - amount;
            }
            reset_animation();
            reset_blackjack();
        }

    }else {
        m_registry->can_click = true;
    }

    (void) delta;
}