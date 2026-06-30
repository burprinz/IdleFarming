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
inline bool try_to_remove(BigNumber &num, BigNumber rem) {
    if (rem <= num) {
        num = num - rem;
        return true;
    }
    return false;
}

inline bool has_enough(BigNumber num, BigNumber rem) {
    return rem <= num;
}
