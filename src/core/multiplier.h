#pragma once
//
// Created by luisb on 06.09.2026.
//

#include <map>

#include "utils/BigNumber.h"

class Multiplier {
public:
    static Multiplier& instance() {
        static Multiplier instance;
        return instance;
    }

    inline void init() noexcept {
        multiplier["full_plants_growth"] = 1;
        multiplier["full_flowers_growth"] = 1;
        multiplier["full_trees_growth"] = 1;
        multiplier["full_cave_machines_growth"] = 1;

        multiplier["skilltree_overall_growth"] = 1;
        multiplier["skilltree_plants_growth"] = 1;
        multiplier["skilltree_flowers_growth"] = 1;
        multiplier["skilltree_trees_growth"] = 1;
        multiplier["skilltree_cave_machines_growth"] = 1;
        recalculate();
    }

    inline void recalculate() noexcept {
        recalculate_full_growth();
    }

    inline void recalculate_full_growth() noexcept {
        multiplier["full_plants_growth"] = multiplier["skilltree_plants_growth"] * multiplier["skilltree_overall_growth"];
        multiplier["full_flowers_growth"] = multiplier["skilltree_flowers_growth"] * multiplier["skilltree_overall_growth"];
        multiplier["full_trees_growth"] = multiplier["skilltree_trees_growth"] * multiplier["skilltree_overall_growth"];
        multiplier["full_cave_machines_growth"] = multiplier["skilltree_cave_machines_growth"] * multiplier["skilltree_overall_growth"];
    }

    inline float get_stat(std::string name) noexcept {
        return multiplier[name];
    }

    inline void higher(std::string name, float value) {
        multiplier[name] += value;
        recalculate();
    }

private:
    std::map<std::string, float> multiplier;
};
