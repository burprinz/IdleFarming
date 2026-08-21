//
// Created by luisb on 10.03.2026.
//


#include <iostream>
#include <nlohmann/adl_serializer.hpp>

#include "window.h"
#include "core/registry.h"
#include <nlohmann/json.hpp>

#include "load.h"
#include "save.h"
#include "../helpers/sizes.h"
#include "helpers/finder.h"
#include "helpers/paths.h"
#include "helpers/util.h"

class EntityFactory {
public:
    static EntityFactory init(Window* window, Registry* registry) noexcept;

    bool read(const std::string& path, nlohmann::json* json) noexcept;
    bool save(const std::string& path, nlohmann::json json) noexcept;

    inline void loadBaseValues() noexcept {

        load_item_categories();

        load_symbols();

        load_base_fields();

        create_tools();

        create_helpers();

        create_achievements();


        int time_diff = static_cast<int>(load_time_difference());
        std::cout<<"Offline progress time: "<<time_diff<<"s"<<std::endl;

        load_stats();
        load_unlocked_plants();
        load_money();
        load_item_amounts();
        load_tools();
        load_done_achievements();
        load_fields(time_diff);
        load_helpers(time_diff);
    }

    inline void save_values() noexcept {
        save_time();
        save_unlocked_plants();
        save_money();
        save_item_amounts();
        save_tools();
        save_fields();
        save_helpers();
        save_stats();
        save_done_achievements();
    }


private:
    Registry* m_registry;
    Window* m_window;

    std::string texture_path = getBasePath() + "/assets/textures/";

    nlohmann::json m_json;

    nlohmann::json m_levelJson;
    nlohmann::json m_objectJson;

    void load_symbols() noexcept;

    inline void load_item_categories() noexcept {
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

    inline void load_base_fields() noexcept {
        std::string json_path = "/assets/data/basevalues/fields.json";
        nlohmann::json fields_json;
        bool loaded = read(json_path, &fields_json);
        if (!loaded) {
            std::cout<<"Error loading Field List"<<std::endl;
        }

        for (auto& field : fields_json["fields"]) {
            std::string field_name = field["name"].get<std::string>();
            std::string field_type = field["type"].get<std::string>();
            bool is_tree = field["is_tree"].get<bool>();
            bool has_inv = field["has_inv"].get<bool>();

            std::string raw_plant_type = field["plant_type"].get<std::string>();
            int plant_type = -1;
            if (raw_plant_type == "normal") {
                plant_type = NORMAL_PLANT;
            } else if (raw_plant_type == "collect") {
                plant_type = COLLECT_PLANT;
            } else if (raw_plant_type == "inventory") {
                plant_type = INVENTORY_PLANT;
            }

            entt::entity id = m_registry->ecs.create();

            Field f;
            f.name = field_name;
            f.type = field_type;
            f.plant_type = plant_type;
            f.list_id = m_registry->field_order.size();

            load_field_plants(&f, is_tree, has_inv);

            m_registry->ecs.emplace<Field>(id, f);

            m_registry->current_plant_tools[field_type] = entt::null;
            m_registry->current_harvest_tools[field_type] = entt::null;
            m_registry->current_collect_tools[field_type] = entt::null;
            m_registry->tool_work_area_names[field_type] = "???";
            m_registry->field_helper[id] = entt::null;

            // The field order for the field selection
            m_registry->field_order.push_back(id);
            m_registry->field_ids[f.name] = m_registry->field_order.size()-1;
        }
    }

    inline void load_field_plants(Field *f, bool is_tree, bool has_inv) noexcept {
        std::string json_path = "/assets/data/basevalues/field_infos/" + f->name + ".json";
        nlohmann::json plant_list_json;
        bool loaded = read(json_path, &plant_list_json);
        if (!loaded) {
            std::cout<<"Error loading Field List"<<std::endl;
        }

        f->display_name = plant_list_json["name"];

        // The shown field color
        m_registry->field_colors.push_back(extract_color(plant_list_json["color"]));

        // Every order the plants should be shown as buttons
        std::vector<entt::entity> plant_order;

        for (auto& plant : plant_list_json["plants"]) {
            std::string plant_name = plant.get<std::string>();
            std::string plant_path = "/assets/data/basevalues/plantables/" + f->name + "/" + plant_name + ".json";

            nlohmann::json plant_json;
            loaded = read(plant_path, &plant_json);
            if (!loaded) {
                std::cout<<"Error loading " << plant_name << " in EntityFactory with path <"<<plant_path<<">"<<std::endl;
                continue;
            }
            create_plant_like(plant_json, plant_order, is_tree, has_inv);
        }

        m_registry->plant_order.push_back(plant_order);
        m_registry->last_selected_plant.push_back(entt::null);
        m_registry->current_selected_plant.push_back(entt::null);

        std::vector<std::vector<entt::entity>> field_tiles;
        create_field(field_tiles, is_tree || has_inv);
        m_registry->plant_field_tiles.push_back(field_tiles);
    }

    inline void create_field(std::vector<std::vector<entt::entity>> &tiles, bool is_regrowable) noexcept {
        tiles.clear();

        for (int y = 0; y < normal_filed_height; y++) {
            std::vector<entt::entity> row;
            for (int x = 0; x < normal_field_width; x++) {
                entt::entity fID = m_registry->ecs.create();

                FarmField f;
                f.x = x;
                f.y = y;
                f.unlocked = true;
                f.is_regrowable_field = is_regrowable;
                m_registry->ecs.emplace<FarmField>(fID, f);

                glm::vec2 pos = {0.5 + x*field_size, 0.5 + y*field_size};
                m_registry->ecs.emplace<Position>(fID, pos);
                glm::vec2 size = {field_size, field_size};
                m_registry->ecs.emplace<Dimension>(fID, size);

                row.push_back(fID);
            }
            tiles.push_back(row);
        }
    }

    void create_plant_like(nlohmann::json plant, std::vector<entt::entity> &order, bool is_tree, bool has_inv) noexcept;

    inline void create_tools() noexcept {
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

    inline void load_tool_list(nlohmann::json tool_list) {
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

    inline void create_helpers() {
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

    inline void load_helper(nlohmann::json raw_helper_infos, std::string type) {

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

    inline void create_achievements() {
        std::string json_path = "/assets/data/basevalues/achievement_types.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading Achievement type List"<<std::endl;
            return;
        }

        for (const auto& type : json["types"]) {
            create_achievement_type(type.get<std::string>());
        }
    }

    inline void create_achievement_type(std::string name) {
        std::string json_path = "/assets/data/basevalues/achievements/"+name+".json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading Achievement type List"<<std::endl;
            return;
        }

        std::string base_name = json["name"].get<std::string>();
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
            a.done = false;

            entt::entity id = m_registry->ecs.create();
            m_registry->ecs.emplace<Achievement>(id, a);

            m_registry->achievement_entities[a.name] = id;
            c += 1;
        }
    }


    inline void save_time() noexcept {
        nlohmann::json j;
        j["time"] = get_current_timestamp();
        save("save_time.json", j);

    }

    inline long long load_time_difference() noexcept {
        long long start_time = load_time();
        long long current_time = get_current_timestamp();
        return current_time - start_time;
    }

    inline long long load_time() noexcept {
        std::string json_path = save_path + "save_time.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading money"<<std::endl;
            return get_current_timestamp();
        }

        return json["time"].get<long long>();
    }

    inline void save_money() noexcept {
        BigNumber money = m_registry->ecs.get<Amount>(m_registry->money()).am;
        nlohmann::json result = {{"money", create_bignumber_json(money)}};
        save("money.json", result);
    }

    inline void load_money() noexcept {
        std::string json_path = save_path + "money.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading money"<<std::endl;
            return;
        }

        BigNumber am = extract_bignumber(json["money"]);
        Amount a;
        a.am = am;

        m_registry->ecs.emplace_or_replace<Amount>(m_registry->money(), a);
    }

    inline void save_item_amounts() noexcept {
        std::vector<nlohmann::json> item_list;
        for (entt::entity item_id : m_registry->ecs.view<Item>()) {
            if (m_registry->ecs.all_of<Amount>(item_id)) {
                item_list.push_back(create_item_amount_json(m_registry, item_id));
            }
        }
        nlohmann::json results = nlohmann::json::object({
            {"list", item_list}
        });
        save("item_amounts.json", results);
    }

    inline void load_item_amounts() noexcept {
        std::string json_path = save_path + "item_amounts.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading item amounts"<<std::endl;
            return;
        }

        for (const auto& item : json["list"]) {
            std::string name = item["name"].get<std::string>();
            BigNumber am = extract_bignumber(item["amount"]);
            Amount amount;
            amount.am = am;
            entt::entity item_entity = m_registry->item_entities[name];
            m_registry->ecs.emplace_or_replace<Amount>(item_entity, amount);
        }
    }

    inline void save_tools() noexcept {
        nlohmann::json unlocked_tools = create_unlocked_tools_json(m_registry);
        nlohmann::json activated_tools = create_activated_tools_json(m_registry);

        nlohmann::json result = {{"unlocked", unlocked_tools}, {"activated", activated_tools}};
        save("tools.json", result);
    }

    inline void load_tools() noexcept {
        std::string json_path = save_path + "tools.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading item amounts"<<std::endl;
            return;
        }

        for (const auto& unlocked : json["unlocked"]) {
            std::string name = unlocked.get<std::string>();
            entt::entity id = m_registry->tool_entities[name];
            Unlockable &u = m_registry->ecs.get<Unlockable>(id);
            u.unlocked = true;
        }

        for (const auto& activated : json["activated"]) {
            std::string name = activated.get<std::string>();
            entt::entity id = m_registry->tool_entities[name];
            activate_tool(m_registry, id);
        }
    }

    inline void save_fields() noexcept {

        std::vector<nlohmann::json> fields;

        for (size_t i = 0; i < m_registry->field_order.size(); i++) {
            entt::entity field_id = m_registry->field_order[i];
            fields.push_back(create_field_json(m_registry, i, field_id));
        }

        nlohmann::json result;
        result["fields"] = fields;

        save("fields.json", result);

    }

    inline void load_fields(int time_diff) noexcept {
        std::string json_path = save_path + "fields.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading item amounts"<<std::endl;
            return;
        }

        for (const auto& field_json : json["fields"]) {
            std::string name = field_json["name"].get<std::string>();
            nlohmann::json plant_list = field_json["plants"];
            size_t field_id = m_registry->field_ids[name];
            load_current_field_plants(m_registry, plant_list, field_id, time_diff);
        }
    }

    inline void save_helpers() noexcept {
        std::vector<nlohmann::json> helpers;

        for (entt::entity id : m_registry->ecs.view<FarmHelper>()) {
            if (m_registry->ecs.get<Unlockable>(id).unlocked) {
                helpers.push_back(create_helper_json(m_registry, id));
            }
        }

        nlohmann::json result;
        result["helpers"] = helpers;

        save("helpers.json", result);
    }

    inline void load_helpers(int time_diff) noexcept {
        std::string json_path = save_path + "helpers.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading item amounts"<<std::endl;
            return;
        }

        for (const auto& helper : json["helpers"]) {
            load_current_helpers(m_registry, helper, time_diff);
        }
    }

    inline void save_unlocked_plants() noexcept {
        nlohmann::json result;

        std::vector<std::string> unlocked_plants;
        for (entt::entity id : m_registry->ecs.view<Plant>()) {
            if (m_registry->ecs.get<Unlockable>(id).unlocked) {
                unlocked_plants.push_back(m_registry->ecs.get<Item>(id).name);
            }
        }

        result["plants"] = unlocked_plants;
        save("unlocked_plants.json", result);
    }

    inline void load_unlocked_plants() noexcept {
        std::string json_path = save_path + "unlocked_plants.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading item amounts"<<std::endl;
            return;
        }

        for (const auto& plant : json["plants"]) {
            entt::entity id = m_registry->item_entities[plant.get<std::string>()];
            Unlockable &u = m_registry->ecs.get<Unlockable>(id);
            u.unlocked = true;
        }
    }

    inline void save_stats() noexcept {
        nlohmann::json result = create_stats_json();
        save("stats.json", result);
    }

    inline void load_stats() noexcept {
        std::string json_path = save_path + "stats.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading stats"<<std::endl;
            return;
        }
        load_current_stats(json);
    }

    inline void save_done_achievements() noexcept {
        std::vector<std::string> done;
        for (entt::entity id : m_registry->ecs.view<Achievement>()) {
            Achievement a = m_registry->ecs.get<Achievement>(id);
            if (a.done) {
                done.push_back(a.name);
            }
        }
        nlohmann::json result;
        result["done"] = done;
        save("done_achievements.json", result);
    }

    inline void load_done_achievements() noexcept {
        std::string json_path = save_path + "done_achievements.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading done achievements"<<std::endl;
            return;
        }

        for (const auto& name : json["done"]) {
            m_registry->ecs.get<Achievement>(m_registry->achievement_entities[name.get<std::string>()]).done = true;
        }
    }
};
