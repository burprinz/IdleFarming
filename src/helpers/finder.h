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

inline entt::entity get_field_id_by_type(Registry* m_registry, std::string type) {
    for (entt::entity id : m_registry->ecs.view<Field>()) {
        if (m_registry->ecs.get<Field>(id).type == type) {
            return id;
        }
    }
    std::cout<<"Not found: "<<type<<std::endl;
    return entt::null;
}

inline entt::entity get_item_category_by_name(Registry* m_registry, std::string name) {
    for (entt::entity id : m_registry->ecs.view<ItemCategory>()) {
        if (m_registry->ecs.get<ItemCategory>(id).id_name == name) {
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

template<typename T> inline bool is_in_list(std::vector<T> &list, T value) noexcept{
    for (T item : list) {
        if (item == value) return true;
    }
    return false;
}

inline bool key_in_unordered_map(const std::unordered_map<std::string, std::string>& map, const std::string& key) {
    return map.contains(key);
}

inline std::string get_lang_if_in_map(std::unordered_map<std::string, std::string> &lang_map, const std::string& key) {
    auto i = lang_map.find(key);
    return i != lang_map.end() ? i->second : key;
}