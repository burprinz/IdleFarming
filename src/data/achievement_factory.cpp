//
// Created by luisb on 25.09.2026.
//

#include "entity_factory.h"

void EntityFactory::create_achievements() noexcept {
    std::string json_path = "/assets/data/basevalues/achievement_types.json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading Achievement type List"<<std::endl;
        return;
    }

    m_registry->achievements_order.clear();

    for (const auto& type : json["types"]) {
        create_achievement_type(type.get<std::string>());
    }
}

void EntityFactory::create_achievement_type(std::string name) noexcept {
        std::string json_path = "/assets/data/basevalues/achievements/"+name+".json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading Achievement type List"<<std::endl;
            return;
        }

        TextureInformation texture = create_achievement_texture(name);

        std::string base_name = json["name"].get<std::string>();

        if (json["with_stats"].get<bool>()) {
            std::string pre_desc = json["desc_pre"].get<std::string>();
            std::string post_desc = json["desc_post"].get<std::string>();
            std::vector<std::string> needed_stat_types = json["counting_stats"].get<std::vector<std::string>>();
            int c = 1;
            for (const auto& amount : json["amounts"]) {
                BigNumber am = extract_bignumber(amount);

                Achievement a;
                a.name = base_name + " " + create_roman_num(c);
                a.description = pre_desc + " " + am.toString() + " " + post_desc;
                a.amount = am;
                a.needed_stat_types = needed_stat_types;
                a.with_stats = true;
                a.done = false;

                entt::entity id = m_registry->ecs.create();
                m_registry->ecs.emplace<Achievement>(id, a);
                m_registry->ecs.emplace<TextureInformation>(id, texture);

                m_registry->achievement_entities[a.name] = id;
                m_registry->achievements_order.push_back(id);
                c += 1;
            }
        } else {
            std::string desc = json["desc"].get<std::string>();
            Achievement a;
            a.name = base_name;
            a.description = desc;
            a.with_stats = false;
            a.done = false;

            entt::entity id = m_registry->ecs.create();
            m_registry->ecs.emplace<Achievement>(id, a);
            m_registry->ecs.emplace<TextureInformation>(id, texture);

            m_registry->achievement_entities[a.name] = id;
            m_registry->achievements_order.push_back(id);
        }


    }
