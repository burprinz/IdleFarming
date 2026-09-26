//
// Created by luisb on 25.09.2026.
//

#include "entity_factory.h"

void EntityFactory::create_tools() noexcept {
    m_registry->tool_order.clear();

    std::string json_path = "/assets/data/basevalues/tool_smith.json";
    nlohmann::json type_json;
    bool loaded = read(json_path, &type_json);
    if (!loaded) {
        std::cout<<"Error loading Tool Type List"<<std::endl;
    }

    for (auto& type : type_json["types"]) {
        std::string type_name = type.get<std::string>();
        std::string type_path = "/assets/data/basevalues/tools/" + type_name + ".json";
        nlohmann::json type_info_json;
        loaded = read(type_path, &type_info_json);
        if (!loaded) {
            std::cout<<"Error loading "<<type_name<<" infos"<<std::endl;
        }


        load_tool_list(type_info_json);
    }

}

void EntityFactory::load_tool_list(nlohmann::json tool_list) noexcept {
    std::string category_name = tool_list["category"].get<std::string>();
    std::string work_area = tool_list["area_name"].get<std::string>();

    int usage_id = get_usage_by_name(tool_list["type"].get<std::string>());

    for (auto& t : tool_list["list"]) {
        entt::entity tool_id = m_registry->ecs.create();

        Tool tool;
        tool.field_type = category_name;
        tool.stage = t["stage"].get<int>();
        tool.radius = t["radius"].get<int>();
        tool.usage = usage_id;
        tool.work_area = work_area;
        m_registry->ecs.emplace<Tool>(tool_id, tool);

        Item item;
        item.name = t["id"].get<std::string>();
        item.display_name = t["name"].get<std::string>();
        m_registry->ecs.emplace<Item>(tool_id, item);

        Unlockable u;
        u.unlocked = false;
        m_registry->ecs.emplace<Unlockable>(tool_id, u);

        std::string id_name = t["id"].get<std::string>();
        TextureInformation icon = create_symbol("tools/"+id_name);
        m_registry->ecs.emplace<TextureInformation>(tool_id, icon);

        BuyAmount buy_amount = extract_buyamount(m_registry, t["price"]);
        m_registry->ecs.emplace<BuyAmount>(tool_id, buy_amount);

        m_registry->tool_order.push_back(tool_id);
        m_registry->tool_entities[item.name] = tool_id;
    }

}
