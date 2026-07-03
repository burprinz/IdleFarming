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


void EntityFactory::create_plant_like(nlohmann::json plant, std::vector<entt::entity> &order) noexcept {
    entt::entity plant_id = m_registry->ecs.create();
    Plant p;
    p.name = plant["name"].get<std::string>();
    p.growTime = plant["growtime"].get<float>();
    p.min_collect = plant["collect_amount"]["min"].get<int>();
    p.max_collect = plant["collect_amount"]["max"].get<int>();
    Unlockable unlocked = extract_unlock_infos(plant["unlock"]);
    p.unlocked = unlocked.unlocked;
    m_registry->ecs.emplace<Plant>(plant_id, p);

    m_registry->ecs.emplace<Unlockable>(plant_id, unlocked);


    Color color = extract_color(plant["color"]);
    m_registry->ecs.emplace<Color>(plant_id, color);

    m_registry->ecs.emplace<Amount>(plant_id);

    Price price = extract_price(plant["price"]);
    m_registry->ecs.emplace<Price>(plant_id, price);

    std::string id_name = plant["id_name"].get<std::string>();
    IconTexture it;
    it.info = create_icon(id_name);
    m_registry->ecs.emplace<IconTexture>(plant_id, it);


    order.push_back(plant_id);
}





