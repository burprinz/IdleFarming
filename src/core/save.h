//
// Created by luisb on 05.08.2026.
//

#include <entt/entt.hpp>
#include <nlohmann/adl_serializer.hpp>
#include "registry.h"
#include "stats.h"


inline nlohmann::json create_bignumber_json(BigNumber num) {
    return nlohmann::json::object({
        {"base", num.base},
        {"exp", num.exponent}
    });
}

inline nlohmann::json create_item_amount_json(Registry* m_registry, entt::entity item_id) {
    Item item = m_registry->ecs.get<Item>(item_id);
    Amount am = m_registry->ecs.get<Amount>(item_id);

    std::string name = item.name;
    BigNumber amount = am.am;

    return nlohmann::json::object({
        {"name", name},
        {"amount", create_bignumber_json(amount)}
    });
}

inline nlohmann::json create_unlocked_tools_json(Registry* m_registry) {
    std::vector<std::string> unlocked_tools;

    for (entt::entity tool_id : m_registry->ecs.view<Tool>()) {
        Unlockable u = m_registry->ecs.get<Unlockable>(tool_id);
        if (u.unlocked) {
            Item t = m_registry->ecs.get<Item>(tool_id);
            unlocked_tools.push_back(t.name);
        }
    }

    nlohmann::json j = unlocked_tools;
    return j;
}

inline nlohmann::json create_activated_tools_json(Registry* m_registry) {
    std::vector<std::string> activated_tools;

    for (const auto& [key, tool_id] : m_registry->current_plant_tools) {
        if (tool_id != entt::null) {
            Item t = m_registry->ecs.get<Item>(tool_id);
            activated_tools.push_back(t.name);
        }
    }
    for (const auto& [key, tool_id] : m_registry->current_collect_tools) {
        if (tool_id != entt::null) {
            Item t = m_registry->ecs.get<Item>(tool_id);
            activated_tools.push_back(t.name);
        }
    }
    for (const auto& [key, tool_id] : m_registry->current_harvest_tools) {
        if (tool_id != entt::null) {
            Item t = m_registry->ecs.get<Item>(tool_id);
            activated_tools.push_back(t.name);
        }
    }

    nlohmann::json j = activated_tools;
    return j;
}

inline nlohmann::json create_field_json(Registry* m_registry, size_t i, entt::entity field_id) {
    std::vector<nlohmann::json> plants;

    Field f = m_registry->ecs.get<Field>(field_id);
    std::string field_name = f.name;
    std::vector<std::vector<entt::entity>> field_matrix = m_registry->plant_field_tiles[i];
    for (size_t y = 0; y < field_matrix.size(); y++) {
        std::vector<entt::entity> field_row = field_matrix[y];
        for (size_t x = 0; x < field_row.size(); x++) {
            entt::entity tile_id = field_row[x];
            FarmField tile = m_registry->ecs.get<FarmField>(tile_id);
            if (!tile.is_empty && !tile.helper_active) {
                nlohmann::json current_plant;
                entt::entity plant_id = tile.plant;

                Item item = m_registry->ecs.get<Item>(plant_id);

                Plant plant = m_registry->ecs.get<Plant>(plant_id);

                current_plant["plant_name"] = item.name;
                current_plant["pos"] = {{"x", x}, {"y", y}};
                current_plant["harvestable"] = tile.harvestable;
                current_plant["current_growtime"] = tile.grow;

                if (plant.has_inventory) {
                    Inventory inv = m_registry->ecs.get<Inventory>(tile_id);
                    current_plant["inventory"] = create_bignumber_json(inv.amount);
                }


                plants.push_back(current_plant);
            }
        }
    }

    nlohmann::json j;
    j["name"] =  field_name;
    j["plants"] = plants;

    return j;
}

inline nlohmann::json create_helper_json(Registry* m_registry, entt::entity helper_id) {
    nlohmann::json result;
    std::vector<nlohmann::json> working_area;

    FarmHelper helper = m_registry->ecs.get<FarmHelper>(helper_id);
    for (size_t i = 0; i < helper.working_area.size(); i++) {
        nlohmann::json field_info;

        entt::entity working_plant_entity = helper.working_plants[i];
        entt::entity working_cell_entity = helper.working_area[i];

        FarmField cell = m_registry->ecs.get<FarmField>(working_cell_entity);
        Item item = m_registry->ecs.get<Item>(working_plant_entity);
        Plant plant = m_registry->ecs.get<Plant>(working_plant_entity);

        field_info["pos"] = {{"x", cell.x}, {"y", cell.y}};
        field_info["plant_name"] = item.name;
        field_info["current_growtime"] = cell.grow;

        if (plant.has_inventory) {
            Inventory inv = m_registry->ecs.get<Inventory>(working_cell_entity);
            field_info["inventory"] = create_bignumber_json(inv.amount);
        }

        working_area.push_back(field_info);
    }

    result["helper_name"] = helper.name;
    result["level"] = helper.level;
    result["working_area"] = working_area;

    return result;
}

inline nlohmann::json create_stats_json() noexcept {
    std::vector<nlohmann::json> stats;

    for (const auto& [name, value] : StatsManager::instance().get_map()) {
        nlohmann::json j;
        j["name"] = name;
        j["value"] = create_bignumber_json(value);
        stats.push_back(j);
    }

    nlohmann::json result;
    result["stats"] = stats;
    return result;
}