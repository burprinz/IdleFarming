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