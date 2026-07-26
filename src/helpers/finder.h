#pragma once

//
// Created by luisb on 26.07.2026.
//

inline bool id_in_map(entt::entity id, std::map<std::string, entt::entity> map) {
    for (auto &[key, value] : map) {
        if (id == value) {
            return true;
        }
    }
    return false;
}

inline entt::entity get_item_id_by_name(Registry* m_registry, std::string name) {
    for (entt::entity id : m_registry->ecs.view<Item>()) {
        if (m_registry->ecs.get<Item>(id).name == name) {
            return id;
        }
    }
    std::cout<<"Not found: "<<name<<std::endl;
    return entt::null;
}

inline entt::entity get_field_id_by_name(Registry* m_registry, std::string name) {
    for (entt::entity id : m_registry->ecs.view<Field>()) {
        if (m_registry->ecs.get<Field>(id).name == name) {
            return id;
        }
    }
    std::cout<<"Not found: "<<name<<std::endl;
    return entt::null;
}

inline int get_usage_by_name(std::string name) {
    int usage_id = -1;
    if (name == "planting") {
        usage_id = PLANTING;
    } else if (name == "harvesting") {
        usage_id = HARVESTING;
    }else if (name == "collecting") {
        usage_id = COLLECTING;
    }
    return usage_id;
}