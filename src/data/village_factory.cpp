//
// Created by luisb on 21.09.2026.
//
#include "entity_factory.h"
#include "systems/village.h"

nlohmann::json EntityFactory::load_village_json(std::string name) noexcept {
    std::string json_path = "/assets/data/basevalues/villages/"+name+".json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading Village "<<name<<std::endl;
    }

    return json;
}


void EntityFactory::create_villages() noexcept {
    std::string json_path = "/assets/data/basevalues/villages.json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading Village list"<<std::endl;
    }

    int c = 0;
    entt::entity last_village_id;
    float bottom_y = MIN_Y_OFFSET;
    float top_y = MAX_Y_OFFSET;

    for (const auto& name : json["villages"]) {
        nlohmann::json village_json = load_village_json(name);

        entt::entity entity = m_registry->ecs.create();

        std::string id_name = village_json["id"].get<std::string>();
        glm::vec2 pos = extract_vec2(village_json["pos"]);
        glm::vec2 size = extract_vec2(village_json["size"]);

        Village v;
        v.name = name;
        v.id_name = id_name;

        m_registry->ecs.emplace<Village>(entity, v);
        m_registry->ecs.emplace<Position>(entity, pos);
        m_registry->ecs.emplace<Dimension>(entity, size);

        Unlockable u;
        u.unlocked = false;
        m_registry->ecs.emplace<Unlockable>(entity, u);

        TextureInformation text = create_village_texture(name);
        m_registry->ecs.emplace<TextureInformation>(entity, text);


        if (c > 0) {
            glm::vec2 last_pos = m_registry->ecs.get<Position>(last_village_id).pos;

            top_y = calc_y_middle(last_pos.y, pos.y);

            CloudRange range;
            range.y_min = bottom_y;
            range.y_max = top_y;
            m_registry->ecs.emplace<CloudRange>(last_village_id, range);

            bottom_y = top_y;
        }

        last_village_id = entity;
        c += 1;
    }

    top_y = MAX_Y_OFFSET;
    CloudRange range;
    range.y_min = bottom_y;
    range.y_max = top_y;
    m_registry->ecs.emplace<CloudRange>(last_village_id, range);

}
