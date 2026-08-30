#pragma once

#include <chrono>
#include <common.h>
#include <iostream>
#include "config.h"
#include "../core/registry.h"
#include "window.h"

// Template Functions for often reused Function calls searched for and created with help of ChatGPT

//Non-owning pointer guard
//Helps counter current issues with Pointers no longer existing.
template<typename T>
T& must(T* p, const char* msg) {
    assert(p && msg);
    return *p;
}

//Rect hit-test helper
inline bool point_in_rect(glm::vec2 p, glm::vec2 center, glm::vec2 size) {
    return center.x - size.x/2 <= p.x && p.x <= center.x + size.x/2
        && center.y - size.y/2 <= p.y && p.y <= center.y + size.y/2;
}

//Distance helper
inline float dist(glm::vec2 a, glm::vec2 b) {
    glm::vec2 d = a - b;
    return std::sqrt(d.x*d.x + d.y*d.y);
}

inline bool rectCollision(const Rect& r1, const Rect& r2) {
    return r1.pos.x < r2.pos.x + r2.size.x && r1.pos.x + r1.size.x > r2.pos.x && r1.pos.y < r2.pos.y + r2.size.y && r1.pos.y + r1.size.y > r2.pos.y;
}

inline bool isFullyAboveRect(const glm::vec2 pos, const Rect& belowRect) {
    return pos.y > belowRect.pos.y + belowRect.size.y;
}

inline PolygonShape createConvexPolygon(std::vector<glm::vec2> points) {
    PolygonShape shape;
    shape.vertices = points;

    return shape;
}

inline glm::vec2 calc_center(glm::vec2 top_left, glm::vec2 size) {
    return top_left + (glm::vec2{size.x, -size.y} / 2.0f);
}

inline void activate_tool(Registry* m_registry, entt::entity tool_id) {
    Tool t = m_registry->ecs.get<Tool>(tool_id);

    if (t.usage == PLANTING) {
        m_registry->current_plant_tools[t.field_type] = tool_id;
    } else if (t.usage == HARVESTING) {
        m_registry->current_harvest_tools[t.field_type] = tool_id;
    } else if (t.usage == COLLECTING) {
        m_registry->current_collect_tools[t.field_type] = tool_id;
    }
}

inline long long get_current_timestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

inline std::string create_roman_num(int num) {

    if (num == 0) {
        return "";
    }

    const int values[] = {10, 9, 5, 4, 1};
    const std::string symbols[] = {"X", "IX", "V", "IV", "I"};

    std::string result;
    for (int i = 0; i < 5; ++i) {
        while (num >= values[i]) {
            num -= values[i];
            result += symbols[i];
        }
    }

    return result;
}

inline int get_card_value(std::string name) {
    if (name == "J" || name == "K" || name == "Q") {
        return 10;
    } else if (name == "A") {
        return 11;
    } else {
        return std::atoi(name.c_str());
    }
}

inline int get_card_symbol(std::string name) {
    if (name == "clubs") {
        return CLUBS;
    } else if (name == "diamonds") {
        return DIAMONDS;
    } else if (name == "spades") {
        return SPADES;
    } else if (name == "hearts") {
        return HEARTS;
    }
    return -1;
}

inline std::vector<int> create_chip_list(Registry* m_registry, int bet) {
    int remaining = bet;
    std::vector<int> result;

    for (auto i = m_registry->chip_list.rbegin(); i != m_registry->chip_list.rend(); ++i) {
        if (remaining == 0) {
            break;
        }
        int current_chip_value = *i;
        while (remaining >= current_chip_value) {
            result.push_back(current_chip_value);
            remaining -= current_chip_value;
        }
    }
    return result;
}

template <typename T>
inline void print_list(std::vector<T> list, std::string name="list") {
    std::cout<<name<<"=[";
    for (size_t i = 0; i < list.size(); i++) {
        std::cout<<list[i];
        if (i != list.size()-1) {
            std::cout<<", ";
        }
    }
    std::cout<<"]"<<std::endl;
}

inline int calculate_blackjack_hand(Registry* registry, std::vector<entt::entity> cards) {

    int result = 0;
    int am_aces = 0;
    for (entt::entity card_id : cards) {
        PlayingCard card = registry->ecs.get<PlayingCard>(card_id);
        result += card.value;
        if (card.value == 11) {
            am_aces += 1;
        }
    }

    while (result > 21 && am_aces > 0) {
        result -= 10;
        am_aces--;
    }

    return result;
}