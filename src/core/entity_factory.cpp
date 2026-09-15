//
// Created by luisb on 10.03.2026.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "entity_factory.h"

#include <iostream>

#include "../utils/geometry.h"

EntityFactory EntityFactory::init(Window *window, Registry *registry) noexcept {
    EntityFactory self;
    self.m_window = window;
    self.m_registry = registry;
    return self;
}

bool EntityFactory::read(const std::string& path, nlohmann::json* json_link) noexcept {
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

bool EntityFactory::save(const std::string &path, const nlohmann::json json) noexcept {
    std::string full_path = getBasePath() + save_path + path;

    std::ofstream datei(full_path, std::ios::trunc);
    if (!datei.is_open()) {
        std::cerr << "Fehler: Datei konnte nicht geöffnet werden: " << full_path << std::endl;
        return false;
    }
    try {
        datei << json.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "Fehler beim Serialisieren nach " << full_path << ": " << e.what() << std::endl;
        return false;
    }
    datei.close();
    return true;
}


void EntityFactory::create_plant_like(nlohmann::json plant, std::vector<entt::entity> &order, bool is_tree, bool has_inv) noexcept {
    entt::entity plant_id = m_registry->ecs.create();

    Item i;
    i.name = plant["id_name"].get<std::string>();
    i.display_name = plant["name"].get<std::string>();

    for (auto& cat : plant["categories"]) {
        std::string category_id_name = cat.get<std::string>();
        entt::entity category_id = get_item_category_by_name(m_registry, category_id_name);
        i.categories.push_back(category_id);

        ItemCategory &ic = m_registry->ecs.get<ItemCategory>(category_id);
        ic.items.push_back(plant_id);
    }
    m_registry->ecs.emplace<Item>(plant_id, i);
    m_registry->item_entities[i.name] = plant_id;

    Plant p;
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
    TextureInformation item_text = create_icon(id_name);;
    m_registry->ecs.emplace<TextureInformation>(plant_id, item_text);

    GrowingTexture grow_text;
    if (!is_tree && !has_inv) {
        grow_text.ungrown_text = create_icon(id_name);
    } else {
        grow_text.grown_text = create_icon(id_name + "_grown");
        grow_text.ungrown_text = create_icon(id_name + "_ungrown");
    }
    m_registry->ecs.emplace<GrowingTexture>(plant_id, grow_text);

    order.push_back(plant_id);
    m_registry->market_item_order.push_back(plant_id);


}

void EntityFactory::load_symbols() noexcept {

    m_registry->lock_locked_texture = m_registry->ecs.create();
    TextureInformation lock = create_symbol("lock_locked");
    m_registry->ecs.emplace<TextureInformation>(m_registry->lock_locked_texture, lock);

    m_registry->lock_unlocked_texture = m_registry->ecs.create();
    TextureInformation unlock = create_symbol("lock_unlocked");
    m_registry->ecs.emplace<TextureInformation>(m_registry->lock_unlocked_texture, unlock);

    m_registry->not_found_texture = m_registry->ecs.create();
    TextureInformation not_found = create_symbol("notfound");
    m_registry->ecs.emplace<TextureInformation>(m_registry->not_found_texture, not_found);

}

void EntityFactory::load_item_categories() noexcept {
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

void EntityFactory::load_base_fields() noexcept {
        std::string json_path = "/assets/data/basevalues/fields.json";
        nlohmann::json fields_json;
        bool loaded = read(json_path, &fields_json);
        if (!loaded) {
            std::cout<<"Error loading Field List"<<std::endl;
        }

        bool first = true;
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
            f.growth_multiplier = "full_" + f.name + "_growth";

            Unlockable u;
            u.unlocked = first;
            first = false;//TODO FALSE

            load_field_plants(&f, is_tree, has_inv, id);

            m_registry->ecs.emplace<Field>(id, f);
            m_registry->ecs.emplace<Unlockable>(id, u);


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

void EntityFactory::load_field_plants(Field *f, bool is_tree, bool has_inv, entt::entity parent_id) noexcept {
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
    create_field(field_tiles, is_tree || has_inv, parent_id);
    m_registry->plant_field_tiles.push_back(field_tiles);
}

void EntityFactory::create_field(std::vector<std::vector<entt::entity>> &tiles, bool is_regrowable, entt::entity parent_field_id) noexcept {
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
            f.parent_field = parent_field_id;
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

void EntityFactory::create_playing_cards() noexcept{
    std::string json_path = "/assets/data/basevalues/card_types.json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading Field List"<<std::endl;
    }

    for (const auto& type : json["types"]) {
        int symbol = get_card_symbol(type.get<std::string>());
        for (const auto& num : json["nums"]) {
            entt::entity card_id = m_registry->ecs.create();

            std::string card_name = type.get<std::string>() + "_" + num.get<std::string>();

            TextureInformation texture = create_card_texture(card_name);
            m_registry->ecs.emplace<TextureInformation>(card_id, texture);

            PlayingCard card;
            card.name = card_name;
            card.symbol = symbol;
            card.value = get_card_value(num);
            m_registry->ecs.emplace<PlayingCard>(card_id, card);

            m_registry->ecs.emplace<Position>(card_id);

            m_registry->playing_cards[card_name] = card_id;
            m_registry->card_list.push_back(card_id);
        }
    }

    entt::entity back_card_id = m_registry->ecs.create();
    TextureInformation back_text = create_card_texture("back");
    m_registry->ecs.emplace<TextureInformation>(back_card_id, back_text);
    m_registry->back_card = back_card_id;
}

void EntityFactory::create_poker_chips() noexcept {
    std::string json_path = "/assets/data/basevalues/poker_chips.json";
    nlohmann::json json;
    bool loaded = read(json_path, &json);
    if (!loaded) {
        std::cout<<"Error loading Field List"<<std::endl;
    }

    for (const auto& chips : json["chips"]) {
        entt::entity id = m_registry->ecs.create();
        int value = chips.get<int>();
        TextureInformation texture = create_chip_texture(value);
        m_registry->ecs.emplace<TextureInformation>(id, texture);
        m_registry->poker_chips[value] = id;
        m_registry->chip_list.push_back(value);
    }
}

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