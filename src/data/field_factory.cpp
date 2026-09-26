//
// Created by luisb on 25.09.2026.
//

#include "entity_factory.h"

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
