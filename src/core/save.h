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

