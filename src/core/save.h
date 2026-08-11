//
// Created by luisb on 05.08.2026.
//

#include <entt/entt.hpp>
#include <nlohmann/adl_serializer.hpp>
#include "registry.h"


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
            if (!tile.is_empty) {
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
    j[field_name] = plants;
    return j;
}