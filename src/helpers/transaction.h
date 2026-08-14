#pragma once
#include "utils/BigNumber.h"
//
// Created by luisb on 26.07.2026.
//

/**
 * Tries to remove a value from a BigNum. If it works it also gets removed.
 * @param num the current value
 * @param rem the value to remove
 * @return true when success, false when rem greater than num
 */
inline bool try_to_remove(BigNumber &num, BigNumber rem) noexcept {
    if (rem <= num) {
        num = num - rem;
        return true;
    }
    return false;
}

inline bool try_to_buy(Registry* registry, BuyAmount am, float multiplicator=1.0f) {
    for (size_t i = 0; i < am.amounts.size(); i++) {
        BigNumber having = registry->ecs.get<Amount>(am.item_ids[i]).am;
        BigNumber needed = am.amounts[i] * multiplicator;
        if (having < needed) {
            return false;
        }
    }

    // Von allem genug
    for (size_t i = 0; i < am.amounts.size(); i++) {
        BigNumber &having = registry->ecs.get<Amount>(am.item_ids[i]).am;
        BigNumber needed = am.amounts[i] * multiplicator;
        having = having - needed;
    }
    return true;
}


inline bool try_to_buy(Registry* registry, entt::entity item_id, float multiplicator=1.0f) noexcept {
    BuyAmount am = registry->ecs.get<BuyAmount>(item_id);
    return try_to_buy(registry, am, multiplicator);
}

inline BuyAmount add_multiplicator(BuyAmount am, float multiplicator) {
    BuyAmount result;
    for (size_t i = 0; i < am.amounts.size(); i++) {
        BigNumber needed = am.amounts[i] * multiplicator;
        result.item_ids.push_back(am.item_ids[i]);
        result.amounts.push_back(needed);
    }
    return result;
}

inline bool has_enough(BigNumber num, BigNumber rem) {
    return rem <= num;
}

inline float calc_double_multiplicator(float base, int level) {
    return base * std::pow(2, level);
}