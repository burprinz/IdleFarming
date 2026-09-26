//
// Created by luisb on 25.09.2026.
//

#include "entity_factory.h"

void EntityFactory::preload_skilltree_infos() noexcept {
    std::string json_path = "/assets/data/basevalues/skilltree.json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading Field List"<<std::endl;
    }

    for (const auto& skill : json["skills"]) {
        std::string name = skill["name"].get<std::string>();

        entt::entity id = m_registry->ecs.create();
        m_registry->skilltree_entities[name] = id;

        Skill s;
        s.level = 0;
        s.id_name = name;
        m_registry->ecs.emplace<Skill>(id, s);
        Unlockable u;
        u.unlocked = false;
        m_registry->ecs.emplace<Unlockable>(id, u);
        TextureInformation text = create_skilltree_texture(name);
        m_registry->ecs.emplace<TextureInformation>(id, text);
    }
}
