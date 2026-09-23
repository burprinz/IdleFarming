#pragma once
#include "sizes.h"
#include "core/stats.h"
#include "systems/animation.h"

//
// Created by luisb on 26.07.2026.
//

inline void create_plant_move_animation(entt::entity field_id, entt::entity plant_id, Registry* m_registry, AnimationSystem* m_animation) noexcept {
    if (m_animation == nullptr) {
        return;
    }
    TextureMoveAnimation anim;
    anim.start = m_registry->ecs.get<Position>(field_id).pos;
    anim.end = field_move_animation_end;
    anim.texture = plant_id;
    anim.progress = 0;
    anim.is_finished = false;
    m_animation->add_field_move_animation(m_registry->ecs.get<Field>(m_registry->ecs.get<FarmField>(field_id).parent_field).list_id, anim);
}

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

    inline void harvest_tile(Registry* m_registry, entt::entity field_id, AnimationSystem* m_animation) noexcept {
        FarmField &f = m_registry->ecs.get<FarmField>(field_id);

        if (!f.is_empty && !f.helper_active) {
            entt::entity plant_id = f.plant;
            Plant p = m_registry->ecs.get<Plant>(plant_id);
            BigNumber &am = m_registry->ecs.get<Amount>(plant_id).am;
            // Try to harvest
            if (p.has_inventory) {
                // Machine -> Add inventory am to stash and remove plant and inv
                Inventory inv = m_registry->ecs.get<Inventory>(field_id);
                am += inv.amount;
                m_registry->ecs.remove<Inventory>(field_id);
                f.is_empty = true;
                StatsManager::instance().higher("manual_harvested", 1);
                create_plant_move_animation(field_id, plant_id, m_registry, m_animation);
            } else if (f.harvestable) {
                // No Machine -> Add amount to stash and remove plant
                am += p.min_collect;
                f.harvestable = false;
                f.is_empty = true;
                StatsManager::instance().higher("manual_harvested", 1);
                create_plant_move_animation(field_id, plant_id, m_registry, m_animation);
            }
        }
    }

    inline void collect_tile(Registry* m_registry, entt::entity field_id, bool helper, AnimationSystem* m_animation) noexcept {
        FarmField &f = m_registry->ecs.get<FarmField>(field_id);

        if (!f.is_empty && !f.helper_active) {
            entt::entity plant_id = f.plant;
            Plant p = m_registry->ecs.get<Plant>(plant_id);
            BigNumber &am = m_registry->ecs.get<Amount>(plant_id).am;
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
                create_plant_move_animation(field_id, plant_id, m_registry, m_animation);
            } else if (f.harvestable) {
                // No Machine -> Collect and reset growtime
                am += p.min_collect;
                f.harvestable = false;
                f.grow = p.growTime;
                StatsManager::instance().higher("manual_collected", 1);
                create_plant_move_animation(field_id, plant_id, m_registry, m_animation);
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

    inline void harvest_with_radius(Registry* m_registry, std::vector<std::vector<entt::entity>> field, size_t x, size_t y, int r, AnimationSystem* m_animation) noexcept {
        // Gets called for Every type
        size_t y_start = (static_cast<size_t>(r)>y)?0:(y-r);
        size_t x_start = (static_cast<size_t>(r)>x)?0:(x-r);

        for (size_t y_pos = y_start; y_pos < std::min<size_t>(y+r+1, field.size()); y_pos++) {
            for (size_t x_pos = x_start; x_pos < std::min<size_t>(x+r+1, field[y_pos].size()); x_pos++) {
                entt::entity fieldID = field[y_pos][x_pos];
                harvest_tile(m_registry, fieldID, m_animation);
            }
        }
    }

    inline void collect_with_radius(Registry* m_registry, std::vector<std::vector<entt::entity>> field, size_t x, size_t y, int r, AnimationSystem* m_animation) noexcept {
        // Gets called for Machine or Regrowable
        size_t y_start = (static_cast<size_t>(r)>y)?0:(y-r);
        size_t x_start = (static_cast<size_t>(r)>x)?0:(x-r);

        for (size_t y_pos = y_start; y_pos < std::min<size_t>(y+r+1, field.size()); y_pos++) {
            for (size_t x_pos = x_start; x_pos < std::min<size_t>(x+r+1, field[y_pos].size()); x_pos++) {
                entt::entity fieldID = field[y_pos][x_pos];
                collect_tile(m_registry, fieldID, false, m_animation);
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

                        if (p.regrowTime > 0.0f) {
                            float total_regrow = f.grow - grow_time;
                            long long raw_cycles = static_cast<long long>(total_regrow / p.regrowTime) + 1;

                            BigNumber space = inv.size - inv.amount;
                            double max_cycles_by_space = (p.min_collect > 0)
                                ? std::ceil(space.asDouble() / p.min_collect)
                                : 0.0;

                            long long cycles = std::min<long long>(raw_cycles, static_cast<long long>(max_cycles_by_space));
                            cycles = std::max<long long>(cycles, 0);

                            inv.amount += p.min_collect * static_cast<double>(cycles);
                            if (inv.amount > inv.size) {
                                inv.amount = inv.size;
                            }
                            f.grow -= p.regrowTime * static_cast<float>(cycles);

                            if (inv.amount >= inv.size && f.grow > grow_time) {
                                f.grow = grow_time;
                            }
                        }

                        if (f.grow < p.growTime) {
                            f.grow = p.growTime;
                        }
                    }
                }
            }
        }
    }

    inline void apply_helper_growtime(Registry* m_registry, entt::entity field_id, float delta, AnimationSystem* m_animation) noexcept {
        FarmField &f = m_registry->ecs.get<FarmField>(field_id);
        entt::entity plant_id = f.plant;
        if (!f.is_empty && plant_id != entt::null) {
            Plant p = m_registry->ecs.get<Plant>(plant_id);
        BigNumber &am = m_registry->ecs.get<Amount>(plant_id).am;
        if (!f.is_regrowable_field) {
            float total = f.grow + delta;
            int cycles = static_cast<int>(total / p.growTime);
            float remaining = total - cycles * p.growTime;
            if (cycles > 0) {
                am += p.min_collect * cycles;
                StatsManager::instance().higher("auto_harvested", cycles);
                StatsManager::instance().higher("auto_planted", cycles);
                create_plant_move_animation(field_id, plant_id, m_registry, m_animation);
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
                    create_plant_move_animation(field_id, plant_id, m_registry, m_animation);
                }
                f.grow = p.growTime + rem;
                f.harvestable = false;
            } else {
                Inventory &inv = m_registry->ecs.get<Inventory>(field_id);
                float progress = f.grow - p.growTime;
                float total_regrow = progress + rem_delta;
                int cycles = static_cast<int>(total_regrow / p.regrowTime);
                if (cycles < 0) cycles = 0;
                float remainder = total_regrow - cycles * p.regrowTime;
                if (cycles > 0) {
                    BigNumber n = (inv.size - inv.amount);
                    double needed = n.asDouble();
                    double min_collect_d = p.min_collect;
                    double size_d = inv.size.asDouble();

                    long long steps_to_first_fill =
                        (needed <= 0.0) ? 0 : static_cast<long long>(std::ceil(needed / min_collect_d));

                    if (cycles < steps_to_first_fill) {
                        inv.amount += p.min_collect * static_cast<double>(cycles);
                    } else {
                        long long steps_per_fill = static_cast<long long>(std::ceil(size_d / min_collect_d));
                        long long remaining      = cycles - steps_to_first_fill;
                        long long fills          = 1 + (steps_per_fill > 0 ? remaining / steps_per_fill : 0);
                        long long leftover_steps = steps_per_fill > 0 ? remaining % steps_per_fill : 0;

                        am += inv.size * static_cast<double>(fills);
                        inv.amount = p.min_collect * static_cast<double>(leftover_steps);
                        StatsManager::instance().higher("auto_collected", static_cast<int>(fills));
                        create_plant_move_animation(field_id, plant_id, m_registry, m_animation);

                    }
                }
                /*
                for (int i = 0; i < cycles; ++i) {
                    inv.amount += p.min_collect;
                    if (inv.amount >= inv.size) {
                        am += inv.size;
                        inv.amount = BigNumber(0);
                        StatsManager::instance().higher("auto_collected", 1);
                    }
                }*/
                f.grow = p.growTime + remainder;
                f.harvestable = false;
            }
        }
        }
    }

