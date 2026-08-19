//
// Created by luisb on 05.08.2026.
//


/**
 * For Save-Game Loading
 */


#include "stats.h"
#include "helpers/field_editor.h"
#include "helpers/finder.h"
#include "helpers/sizes.h"
/**
 * For Overall loading
 */
inline glm::vec2 extract_vec2(nlohmann::json raw) noexcept {
    return {raw["x"].get<float>(), raw["y"].get<float>()};
}

inline Color extract_color(nlohmann::json raw) noexcept {
    Color c;
    c.c = {
        raw["r"].get<float>(),
        raw["g"].get<float>(),
        raw["b"].get<float>()};
    return c;
}

inline BigNumber extract_bignumber(nlohmann::json raw) noexcept {
    return BigNumber(raw["base"].get<float>(), raw["exp"].get<int>());
}

inline Unlockable extract_unlock_infos(nlohmann::json raw) noexcept {
    Unlockable info;
    info.unlocked = raw["unlocked"].get<bool>();
    info.price = extract_bignumber(raw["price"]);
    return info;
}

inline Price extract_price(nlohmann::json raw) noexcept {
    Price p;
    p.buy = extract_bignumber(raw["buy"]);
    p.sell = extract_bignumber(raw["sell"]);
    return p;
}

inline BuyAmount extract_buyamount(Registry* m_registry, nlohmann::json price) {
    std::vector<entt::entity> buy_ids;
    std::vector<BigNumber> buy_amounts;
    for (auto &item_price : price) {
        std::string item_name = item_price["name"];
        entt::entity item_id = get_item_id_by_name(m_registry, item_name);
        BigNumber item_amount = extract_bignumber(item_price["amount"]);

        buy_ids.push_back(item_id);
        buy_amounts.push_back(item_amount);
    }

    BuyAmount result;
    result.item_ids = buy_ids;
    result.amounts = buy_amounts;

    return result;
}

inline std::string getTexturePath(std::string relative_path) noexcept {
    return getBasePath() + "/assets/textures/" + relative_path;
}

inline TextureInformation create_icon(std::string name) noexcept {
    TextureInformation info;
    std::string text_path = getTexturePath("icons/" + name + ".png");
    if (std::filesystem::exists(text_path)) {
        info.text = Texture::init(text_path);
    } else {
        info.text = Texture::init(getTexturePath("icons/notfound.png"));
    }
    info.base_size = base_icon_size;
    return info;
}

inline TextureInformation create_symbol(std::string name) {
    TextureInformation info;
    std::string text_path = getTexturePath("symbols/" + name + ".png");
    if (std::filesystem::exists(text_path)) {
        info.text = Texture::init(text_path);
    } else {
        info.text = Texture::init(getTexturePath("symbols/notfound.png"));
    }
    return info;
}

inline TextureInformation create_helper_texture(std::string name) {
    TextureInformation info;
    std::string text_path = getTexturePath("helpers/" + name + ".png");
    if (std::filesystem::exists(text_path)) {
        info.text = Texture::init(text_path);
    } else {
        info.text = Texture::init(getTexturePath("symbols/notfound.png"));
    }
    return info;
}

inline void load_current_field_plants(Registry* m_registry, nlohmann::json plant_list, size_t field_idx, int time_diff) {
    std::vector<std::vector<entt::entity>> &field_tiles = m_registry->plant_field_tiles[field_idx];

    for (const auto& json : plant_list) {
        std::string plant_name = json["plant_name"].get<std::string>();
        size_t x = json["pos"]["x"].get<size_t>();
        size_t y = json["pos"]["y"].get<size_t>();
        bool harvestable = json["harvestable"].get<bool>();
        float current_grow = json["current_growtime"].get<float>();

        entt::entity plant_id = m_registry->item_entities[plant_name];
        entt::entity field_id = field_tiles[y][x];

        Plant plant = m_registry->ecs.get<Plant>(plant_id);
        FarmField &field = m_registry->ecs.get<FarmField>(field_id);

        field.harvestable = harvestable;
        field.grow = current_grow;
        field.is_empty = false;
        field.plant = plant_id;

        if (plant.has_inventory) {
            Inventory inv;
            inv.size = plant.inventory_size;
            inv.amount = extract_bignumber(json["inventory"]);

            m_registry->ecs.emplace<Inventory>(field_id, inv);
        }

        apply_growtime(m_registry, field_id, time_diff);
    }
}

inline void load_current_helpers(Registry* m_registry, nlohmann::json helper_json, int time_diff) noexcept {

    std::string name = helper_json["helper_name"].get<std::string>();
    entt::entity helper_id = m_registry->helper_entities[name];
    FarmHelper &helper = m_registry->ecs.get<FarmHelper>(helper_id);
    helper.level = helper_json["level"].get<int>();
    helper.amount_fields = 10 + helper.level*5;
    if (helper.amount_fields >= normal_field_amount) {
        helper.max_reached = true;
        helper.amount_fields = normal_field_amount;
    }

    Unlockable &u = m_registry->ecs.get<Unlockable>(helper_id);
    u.unlocked = true;


    Field f = m_registry->ecs.get<Field>(helper.field);
    size_t i = f.list_id;

    for (const auto& json : helper_json["working_area"]) {
        std::string plant_name = json["plant_name"].get<std::string>();
        size_t x = json["pos"]["x"].get<size_t>();
        size_t y = json["pos"]["y"].get<size_t>();
        float current_grow = json["current_growtime"].get<float>();
        entt::entity plant_id = m_registry->item_entities[plant_name];
        entt::entity field_id = m_registry->plant_field_tiles[i][y][x];
        Plant plant = m_registry->ecs.get<Plant>(plant_id);
        FarmField &field = m_registry->ecs.get<FarmField>(field_id);
        field.grow = current_grow;
        field.is_empty = false;
        field.plant = plant_id;
        field.helper_active = true;
        if (plant.has_inventory) {
            Inventory inv;
            inv.size = plant.inventory_size;
            inv.amount = extract_bignumber(json["inventory"]);

            m_registry->ecs.emplace<Inventory>(field_id, inv);
        }
        helper.working_area.push_back(field_id);
        helper.working_plants.push_back(plant_id);
        apply_helper_growtime(m_registry, field_id, time_diff);
    }
}

inline void load_current_stats(nlohmann::json json) noexcept {
    for (nlohmann::json stat : json["stats"]) {
        StatsManager::instance().set(stat["name"].get<std::string>(), stat["value"].get<int>());
    }
}