//
// Created by luisb on 21.09.2026.
//
#include "entity_factory.h"

void EntityFactory::create_villages() noexcept {
    std::string json_path = "/assets/data/basevalues/villages.json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading Village list"<<std::endl;
    }

    for (const auto& village : json["villages"]) {
        create_village(village.get<std::string>());
    }
}

void EntityFactory::create_village(std::string name) noexcept {
    std::string json_path = "/assets/data/basevalues/villages/"+name+".json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading Village "<<name<<std::endl;
    }

    entt::entity entity = m_registry->ecs.create();

    std::string id_name = json["id"].get<std::string>();
    glm::vec2 pos = extract_vec2(json["pos"]);
    glm::vec2 size = extract_vec2(json["size"]);

    Village v;
    v.name = name;
    v.id_name = id_name;

    m_registry->ecs.emplace<Village>(entity, v);
    m_registry->ecs.emplace<Position>(entity, pos);
    m_registry->ecs.emplace<Dimension>(entity, size);

    TextureInformation text = create_village_texture(name);
    m_registry->ecs.emplace<TextureInformation>(entity, text);
}

