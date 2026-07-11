//
// Created by luisb on 10.03.2026.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "entity_factory.h"

#include <iostream>

#include "geometry.h"

EntityFactory EntityFactory::init(Window *window, Registry *registry) noexcept {
    EntityFactory self;
    self.m_window = window;
    self.m_registry = registry;
    return self;
}

bool EntityFactory::readPath(const std::string& path, nlohmann::json* json_link) noexcept {
    std::string basePath = getBasePath();
    std::string fullPath = basePath + path;

    // Allows for path to be just the filename
    if (!std::ifstream(fullPath).good())
    {
        fullPath = basePath + "/assets/roomSchemes/" + path;
    }

    //Tries Reading File
    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        fmt::println("Could not open level file: '{}'", fullPath);
        return false;
    }

    //Tries Parsing JSON
    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const std::exception& e)
    {
        fmt::println("Failed to parse JSON from '{}': {}", fullPath, e.what());
        return false;
    }

    //Saves Layout and Path
    //CHATGPT Suggested move as it is better in terms of memory. Also in this StackOverflow: https://stackoverflow.com/questions/3413470/what-is-stdmove-and-when-should-it-be-used
    *json_link = std::move(j);

    //fmt::println("Successfully loaded level file: '{}'", fullPath);
    return true;
}


void EntityFactory::create_plant_like(nlohmann::json plant, std::vector<entt::entity> &order, bool is_tree, bool has_inv) noexcept {
    entt::entity plant_id = m_registry->ecs.create();
    Plant p;
    p.name = plant["name"].get<std::string>();
    p.growTime = plant["growtime"].get<float>();
    p.min_collect = plant["collect_amount"]["min"].get<int>();
    p.max_collect = plant["collect_amount"]["max"].get<int>();
    Unlockable unlocked = extract_unlock_infos(plant["unlock"]);
    p.unlocked = unlocked.unlocked;
    p.is_regrowable = is_tree;
    if (is_tree) {
        p.regrowTime = plant["regrowtime"].get<float>();
    }
    if (plant.contains("has_inventory") && plant["has_inventory"].get<bool>()) {
        p.has_inventory = true;
        p.inventory_size = extract_bignumber(plant["inventory_size"]);
    }
    m_registry->ecs.emplace<Plant>(plant_id, p);

    m_registry->ecs.emplace<Unlockable>(plant_id, unlocked);


    Color color = extract_color(plant["color"]);
    m_registry->ecs.emplace<Color>(plant_id, color);

    m_registry->ecs.emplace<Amount>(plant_id);

    Price price = extract_price(plant["price"]);
    m_registry->ecs.emplace<Price>(plant_id, price);

    std::string id_name = plant["id_name"].get<std::string>();
    IconTexture it;
    if (!is_tree && !has_inv) {
        it.info = create_icon(id_name);
    } else {
        it.info = create_icon(id_name + "_grown");
        it.ungrownInfo = create_icon(id_name + "_ungrown");

        std::cout<<id_name<<std::endl;
    }
    m_registry->ecs.emplace<IconTexture>(plant_id, it);


    order.push_back(plant_id);
}

void EntityFactory::load_symbols() noexcept {

    m_registry->lock_locked_texture = m_registry->ecs.create();
    TextureInformation lock = create_symbol("lock_locked");
    m_registry->ecs.emplace<TextureInformation>(m_registry->lock_locked_texture, lock);

    m_registry->lock_unlocked_texture = m_registry->ecs.create();
    TextureInformation unlock = create_symbol("lock_unlocked");
    m_registry->ecs.emplace<TextureInformation>(m_registry->lock_unlocked_texture, unlock);

}




