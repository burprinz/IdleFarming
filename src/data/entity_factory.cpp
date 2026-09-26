//
// Created by luisb on 10.03.2026.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "entity_factory.h"

#include <iostream>

#include "../core/lang.h"
#include "../utils/geometry.h"

EntityFactory EntityFactory::init(Window *window, Registry *registry) noexcept {
    EntityFactory self;
    self.m_window = window;
    self.m_registry = registry;
    return self;
}


void EntityFactory::load_symbols() noexcept {

    m_registry->lock_locked_texture = m_registry->ecs.create();
    TextureInformation lock = create_symbol("lock_locked");
    m_registry->ecs.emplace<TextureInformation>(m_registry->lock_locked_texture, lock);

    m_registry->lock_unlocked_texture = m_registry->ecs.create();
    TextureInformation unlock = create_symbol("lock_unlocked");
    m_registry->ecs.emplace<TextureInformation>(m_registry->lock_unlocked_texture, unlock);

    m_registry->not_found_texture = m_registry->ecs.create();
    TextureInformation not_found = create_symbol("notfound");
    m_registry->ecs.emplace<TextureInformation>(m_registry->not_found_texture, not_found);

}

void EntityFactory::load_item_categories() noexcept {
        std::string json_path = "/assets/data/basevalues/item_categories.json";
        nlohmann::json cat_json;
        bool loaded = read(json_path, &cat_json);
        if (!loaded) {
            std::cout<<"Error loading Field List"<<std::endl;
        }

        m_registry->market_category_order.clear();

        for (auto& category : cat_json["categories"]) {

            entt::entity id = m_registry->ecs.create();

            ItemCategory ic;
            ic.id_name = category["id"].get<std::string>();
            ic.display_name = category["name"].get<std::string>();

            m_registry->ecs.emplace<ItemCategory>(id, ic);

            m_registry->market_category_order.push_back(id);
        }
    }

void EntityFactory::load_lang(std::string lang) noexcept {
    std::string json_path = "/assets/data/basevalues/lang/" + lang + ".json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading lang "<<lang<<std::endl;
    }

    for (auto& [key, value] : json["multiplier"].items()) {
        Language::instance().multiplier()[key] = value.get<std::string>();
    }

    for (auto& [key, value] : json["buttons"].items()) {
        Language::instance().button()[key] = value.get<std::string>();
    }


}
