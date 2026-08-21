#pragma once
#include "core/stats.h"

//
// Created by luisb on 26.07.2026.
//
    inline void plant_tile(Registry* m_registry, entt::entity field_id, entt::entity plant_id, bool override=false) noexcept{
        FarmField &f = m_registry->ecs.get<FarmField>(field_id);

        if (override || (f.is_empty && !f.helper_active)) {
            // Plant if field is empty and left click pressed
            f.is_empty = false;
            f.plant = plant_id;
            f.grow = 0.0;
            f.harvestable = false;
            Plant p = m_registry->ecs.get<Plant>(plant_id);
            if (p.has_inventory) {
                Inventory inv;
                inv.size = BigNumber(p.inventory_size);
                inv.amount = BigNumber(0);
                m_registry->ecs.emplace_or_replace<Inventory>(field_id, inv);
            }
            if (override) {
                StatsManager::instance().higher("auto_planted", 1);
            } else {
                StatsManager::instance().higher("manual_planted", 1);
            }
        }


    }

    inline void harvest_tile(Registry* m_registry, entt::entity field_id) noexcept {
        FarmField &f = m_registry->ecs.get<FarmField>(field_id);

        if (!f.is_empty && !f.helper_active) {
            Plant p = m_registry->ecs.get<Plant>(f.plant);
            BigNumber &am = m_registry->ecs.get<Amount>(f.plant).am;
            // Try to harvest
            if (p.has_inventory) {
                // Machine -> Add inventory am to stash and remove plant and inv
                Inventory inv = m_registry->ecs.get<Inventory>(field_id);
                am += inv.amount;
                m_registry->ecs.remove<Inventory>(field_id);
                f.is_empty = true;
                StatsManager::instance().higher("manual_harvested", 1);
            } else if (f.harvestable) {
                // No Machine -> Add amount to stash and remove plant
                am += p.min_collect;
                f.harvestable = false;
                f.is_empty = true;
                StatsManager::instance().higher("manual_harvested", 1);
            }
        }
    }

    inline void collect_tile(Registry* m_registry, entt::entity field_id, bool helper) noexcept {
        FarmField &f = m_registry->ecs.get<FarmField>(field_id);

        if (!f.is_empty && !f.helper_active) {
            Plant p = m_registry->ecs.get<Plant>(f.plant);
            BigNumber &am = m_registry->ecs.get<Amount>(f.plant).am;
            // Try to collect
            if (p.has_inventory && m_registry->ecs.all_of<Inventory>(field_id)) {
                // Machine -> Collect inventory and keep everything
                Inventory &inv = m_registry->ecs.get<Inventory>(field_id);
                // Wenn nicht Helper
                if (!helper) {
                    am += inv.amount;
                    inv.amount = BigNumber(0);
                    StatsManager::instance().higher("manual_collected", 1);
                }
            } else if (f.harvestable) {
                // No Machine -> Collect and reset growtime
                am += p.min_collect;
                f.harvestable = false;
                f.grow = p.growTime;
                StatsManager::instance().higher("manual_collected", 1);
            }
        }
    }



    inline void plant_with_radius(Registry* m_registry, std::vector<std::vector<entt::entity>> field, entt::entity p_id, size_t x, size_t y, int r) noexcept {

        size_t y_start = (static_cast<size_t>(r)>y)?0:(y-r);
        size_t x_start = (static_cast<size_t>(r)>x)?0:(x-r);

        for (size_t y_pos = y_start; y_pos < std::min<size_t>(y+r+1, field.size()); y_pos++) {
            for (size_t x_pos = x_start; x_pos < std::min<size_t>(x+r+1, field[y_pos].size()); x_pos++) {
                entt::entity fieldID = field[y_pos][x_pos];
                plant_tile(m_registry, fieldID, p_id, false);
            }
        }

    }

    inline void harvest_with_radius(Registry* m_registry, std::vector<std::vector<entt::entity>> field, size_t x, size_t y, int r) noexcept {
        // Gets called for Every type
        size_t y_start = (static_cast<size_t>(r)>y)?0:(y-r);
        size_t x_start = (static_cast<size_t>(r)>x)?0:(x-r);

        for (size_t y_pos = y_start; y_pos < std::min<size_t>(y+r+1, field.size()); y_pos++) {
            for (size_t x_pos = x_start; x_pos < std::min<size_t>(x+r+1, field[y_pos].size()); x_pos++) {
                entt::entity fieldID = field[y_pos][x_pos];
                harvest_tile(m_registry, fieldID);
            }
        }
    }

    inline void collect_with_radius(Registry* m_registry, std::vector<std::vector<entt::entity>> field, size_t x, size_t y, int r) noexcept {
        // Gets called for Machine or Regrowable
        size_t y_start = (static_cast<size_t>(r)>y)?0:(y-r);
        size_t x_start = (static_cast<size_t>(r)>x)?0:(x-r);

        for (size_t y_pos = y_start; y_pos < std::min<size_t>(y+r+1, field.size()); y_pos++) {
            for (size_t x_pos = x_start; x_pos < std::min<size_t>(x+r+1, field[y_pos].size()); x_pos++) {
                entt::entity fieldID = field[y_pos][x_pos];
                collect_tile(m_registry, fieldID, false);
            }
        }
    }

    inline void apply_growtime(Registry* m_registry, entt::entity field_id, float delta) noexcept {
        FarmField &f = m_registry->ecs.get<FarmField>(field_id);
        if (!f.is_empty && f.plant != entt::null) {
            Plant p = m_registry->ecs.get<Plant>(f.plant);

            if (!f.is_regrowable_field) {
                // Flower growth
                if (f.grow < p.growTime) {
                    f.grow += delta;
                } else {
                    f.grow = p.growTime;
                    f.harvestable = true;
                }
            } else {
                // Tree / Machine growth
                float grow_time = p.growTime + p.regrowTime;
                f.grow += delta;

                if (f.grow >= grow_time) {
                    if (!p.has_inventory) {
                        f.grow = grow_time;
                        f.harvestable = true;
                    } else {
                        Inventory &inv = m_registry->ecs.get<Inventory>(field_id);

                        int cycles = 0;
                        while (f.grow >= grow_time && inv.amount < inv.size) {
                            f.grow -= p.regrowTime;
                            cycles++;
                        }

                        inv.amount += p.min_collect * cycles;
                        if (inv.amount > inv.size) {
                            inv.amount = inv.size;
                        }

                        if (f.grow < p.growTime) {
                            f.grow = p.growTime;
                        }
                    }
                }
            }
        }
    }

    inline void apply_helper_growtime(Registry* m_registry, entt::entity field_id, float delta) noexcept {
        FarmField &f = m_registry->ecs.get<FarmField>(field_id);
        if (!f.is_empty && f.plant != entt::null) {
            Plant p = m_registry->ecs.get<Plant>(f.plant);
        BigNumber &am = m_registry->ecs.get<Amount>(f.plant).am;
        if (!f.is_regrowable_field) {
            float total = f.grow + delta;
            int cycles = static_cast<int>(total / p.growTime);
            float remaining = total - cycles * p.growTime;
            if (cycles > 0) {
                am += p.min_collect * cycles;
                StatsManager::instance().higher("auto_harvested", cycles);
                StatsManager::instance().higher("auto_planted", cycles);
            }
            f.grow = remaining;
            f.harvestable = false;
        } else {
            float rem_delta = delta;
            if (f.grow < p.growTime) {
                float def = p.growTime - f.grow;
                if (rem_delta < def) {
                    f.grow += rem_delta;
                    return;
                }
                rem_delta -= def;
                f.grow = p.growTime;
            }
            if (!p.has_inventory) {
                float progress = f.grow - p.growTime;
                float total_regrow = progress + rem_delta;
                int cycles = static_cast<int>(total_regrow / p.regrowTime);
                float rem = total_regrow - cycles * p.regrowTime;
                if (cycles > 0) {
                    am += p.min_collect * cycles;
                    StatsManager::instance().higher("auto_collected", cycles);
                }
                f.grow = p.growTime + rem;
                f.harvestable = false;
            } else {
                Inventory &inv = m_registry->ecs.get<Inventory>(field_id);
                float progress = f.grow - p.growTime;
                float total_regrow = progress + rem_delta;
                int cycles = static_cast<int>(total_regrow / p.regrowTime);
                float remainder = total_regrow - cycles * p.regrowTime;
                for (int i = 0; i < cycles; ++i) {
                    inv.amount += p.min_collect;
                    if (inv.amount >= inv.size) {
                        am += inv.size;
                        inv.amount = BigNumber(0);
                        StatsManager::instance().higher("auto_collected", 1);
                    }
                }
                f.grow = p.growTime + remainder;
                f.harvestable = false;
            }
        }
        }
    }