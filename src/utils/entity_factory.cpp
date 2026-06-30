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

    fmt::println("Successfully loaded level file: '{}'", fullPath);
    return true;
}

void EntityFactory::createFromFile(const std::string &path) noexcept {
    (void) path;
}

void EntityFactory::loadBasePlants() noexcept {
    std::string json_path = "/assets/data/basevalues/plants.json";
    bool loaded = readPath(json_path, &m_json);
    if (!loaded) {
        std::cout<<"Error loading base plants in EntityFactory"<<std::endl;
    }

    for (auto& plant : m_json["plants"]) {

        entt::entity plant_id = m_registry->ecs.create();
        Plant p;
        p.name = plant["name"].get<std::string>();
        p.growTime = plant["growtime"].get<float>();
        p.min_collect = plant["collect_amount"]["min"].get<int>();
        p.max_collect = plant["collect_amount"]["max"].get<int>();
        p.unlocked = plant["unlocked"].get<bool>();
        m_registry->ecs.emplace<Plant>(plant_id, p);

        Color color = extract_color(plant["color"]);
        m_registry->ecs.emplace<Color>(plant_id, color);

        std::string id_name = plant["id_name"].get<std::string>();
        m_registry->plants[id_name] = plant_id;
        m_registry->plant_id_names[plant_id] = id_name;

        m_registry->plant_ids.push_back(id_name);

        m_registry->ecs.emplace<Amount>(plant_id);

        Price price = extract_price(plant["price"]);
        m_registry->ecs.emplace<Price>(plant_id, price);

        IconTexture it;
        it.info = create_icon(id_name);
        m_registry->ecs.emplace<IconTexture>(plant_id, it);

        Unlockable unlocked = extract_unlock_infos(plant["unlock"]);
        m_registry->ecs.emplace<Unlockable>(plant_id, unlocked);
    }

    std::cout<<"Plants loaded"<<std::endl;
}






