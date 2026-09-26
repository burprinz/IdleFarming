//
// Created by luisb on 25.09.2026.
//

#include "entity_factory.h"

void EntityFactory::create_helpers() noexcept {
    std::string json_path = "/assets/data/basevalues/farm_helpers.json";
    nlohmann::json type_json;
    bool loaded = read(json_path, &type_json);
    if (!loaded) {
        std::cout<<"Error loading Tool Type List"<<std::endl;
    }

    for (auto& type : type_json["types"]) {
        std::string type_name = type.get<std::string>();
        std::string type_path = "/assets/data/basevalues/farm_helpers/" + type_name + ".json";
        nlohmann::json type_info_json;
        loaded = read(type_path, &type_info_json);
        if (!loaded) {
            std::cout<<"Error loading "<<type_name<<" infos"<<std::endl;
        }

        load_helper(type_info_json, type_name);
    }
}

void EntityFactory::load_helper(nlohmann::json raw_helper_infos, std::string type) noexcept{

    entt::entity helper_id = m_registry->ecs.create();
    m_registry->helpers_order.push_back(helper_id);

    FarmHelper helper;
    helper.name = raw_helper_infos["name"].get<std::string>();
    helper.field = get_field_id_by_name(m_registry, type);
    m_registry->ecs.emplace<FarmHelper>(helper_id, helper);

    m_registry->field_helper[helper.field] = helper_id;

    Unlockable unlock;
    unlock.unlocked = false;
    m_registry->ecs.emplace<Unlockable>(helper_id, unlock);

    BuyAmount buyam = extract_buyamount(m_registry, raw_helper_infos["price"]);
    m_registry->ecs.emplace<BuyAmount>(helper_id, buyam);

    TextureInformation texture = create_helper_texture(helper.name);
    m_registry->ecs.emplace<TextureInformation>(helper_id, texture);

    m_registry->helper_entities[helper.name] = helper_id;
}
