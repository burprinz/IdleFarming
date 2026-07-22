#pragma once
#include "utils/BigNumber.h"
//
// Created by luisb on 30.06.2026.
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

inline bool try_to_buy(Registry* registry, entt::entity item_id) noexcept {

    BuyAmount am = registry->ecs.get<BuyAmount>(item_id);

    for (size_t i = 0; i < am.amounts.size(); i++) {
        BigNumber having = registry->ecs.get<Amount>(am.item_ids[i]).am;
        std::cout<<having.toString()<<" "<<am.amounts[i].toString()<<std::endl;
        if (having < am.amounts[i]) {
            return false;
        }
    }

    // Von allem genug
    for (size_t i = 0; i < am.amounts.size(); i++) {

        BigNumber &having = registry->ecs.get<Amount>(am.item_ids[i]).am;
        having = having - am.amounts[i];
    }
    return true;
}

inline bool has_enough(BigNumber num, BigNumber rem) {
    return rem <= num;
}

inline bool id_in_map(entt::entity id, std::map<std::string, entt::entity> map) {
    for (auto &[key, value] : map) {
        if (id == value) {
            return true;
        }
    }
    return false;
}
