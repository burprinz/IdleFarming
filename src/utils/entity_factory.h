//
// Created by luisb on 10.03.2026.
//


#include <iostream>
#include <nlohmann/adl_serializer.hpp>

#include "window.h"
#include "core/registry.h"
#include <nlohmann/json.hpp>

#include "core/sizes.h"

class EntityFactory {
public:
    static EntityFactory init(Window* window, Registry* registry) noexcept;

    bool readPath(const std::string& path, nlohmann::json* json) noexcept;

    inline void loadBaseValues() noexcept {
        load_symbols();

        load_base_fields();

        create_tools();
    }


private:
    Registry* m_registry;
    Window* m_window;

    std::string m_texture_base_path = getBasePath() + "/assets/textures/";

    nlohmann::json m_json;

    nlohmann::json m_levelJson;
    nlohmann::json m_objectJson;

    inline void load_base_fields() noexcept {
        std::string json_path = "/assets/data/basevalues/fields.json";
        nlohmann::json fields_json;
        bool loaded = readPath(json_path, &fields_json);
        if (!loaded) {
            std::cout<<"Error loading Field List"<<std::endl;
        }

        for (auto& field : fields_json["fields"]) {
            std::string field_name = field["name"].get<std::string>();
            std::string field_type = field["type"].get<std::string>();
            bool is_tree = field["is_tree"].get<bool>();
            bool has_inv = field["has_inv"].get<bool>();


            entt::entity id = m_registry->ecs.create();

            // The field order for the field selection
            m_registry->field_order.push_back(id);

            Field f;
            f.name = field_name;
            f.type = field_type;

            load_field_plants(&f, is_tree, has_inv);

            m_registry->ecs.emplace<Field>(id, f);

            m_registry->current_plant_tools[field_type] = entt::null;
            m_registry->current_harvest_tools[field_type] = entt::null;

        }
    }

    inline void load_field_plants(Field *f, bool is_tree, bool has_inv) noexcept {
        std::string json_path = "/assets/data/basevalues/field_infos/" + f->name + ".json";
        nlohmann::json plant_list_json;
        bool loaded = readPath(json_path, &plant_list_json);
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
            loaded = readPath(plant_path, &plant_json);
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

        for (int y = 0; y < 15; y++) {
            std::vector<entt::entity> row;
            for (int x = 0; x < 10; x++) {
                entt::entity fID = m_registry->ecs.create();

                FarmField f;
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
        std::string json_path = "/assets/data/basevalues/tool_smith.json";
        nlohmann::json type_json;
        bool loaded = readPath(json_path, &type_json);
        if (!loaded) {
            std::cout<<"Error loading Tool Type List"<<std::endl;
        }

        for (auto& type : type_json["types"]) {
            std::string type_name = type.get<std::string>();
            std::string type_path = "/assets/data/basevalues/tools/" + type_name + ".json";
            nlohmann::json type_info_json;
            loaded = readPath(type_path, &type_info_json);
            if (!loaded) {
                std::cout<<"Error loading "<<type_name<<" infos"<<std::endl;
            }

            load_tool_list(type_info_json);
        }

    }

    inline void load_tool_list(nlohmann::json tool_list) {
        std::string category_name = tool_list["category"].get<std::string>();

        bool is_planting = tool_list["type"].get<std::string>() == "planting";

        for (auto& t : tool_list["list"]) {
            entt::entity tool_id = m_registry->ecs.create();

            Tool tool;
            tool.field_type = category_name;
            tool.stage = t["stage"].get<int>();
            tool.radius = t["radius"].get<int>();
            tool.is_for_planting = is_planting;
            m_registry->ecs.emplace<Tool>(tool_id, tool);

            Item item;
            item.name = t["name"].get<std::string>();
            m_registry->ecs.emplace<Item>(tool_id, item);

            Unlockable u;
            u.unlocked = false;
            m_registry->ecs.emplace<Unlockable>(tool_id, u);

            std::string id_name = t["id"].get<std::string>();
            TextureInformation icon = create_icon(id_name);
            m_registry->ecs.emplace<TextureInformation>(tool_id, icon);

            std::vector<entt::entity> buy_ids;
            std::vector<BigNumber> buy_amounts;

            for (auto &item_price : t["price"]) {
                std::string item_name = item_price["name"];
                entt::entity item_id = get_item_id_by_name(item_name);
                BigNumber item_amount = extract_bignumber(item_price["amount"]);

                buy_ids.push_back(item_id);
                buy_amounts.push_back(item_amount);
            }

            BuyAmount buy_amount;
            buy_amount.item_ids = buy_ids;
            buy_amount.amounts = buy_amounts;
            m_registry->ecs.emplace<BuyAmount>(tool_id, buy_amount);
        }

    }

    inline entt::entity get_item_id_by_name(std::string name) {
        for (entt::entity id : m_registry->ecs.view<Item>()) {
            if (m_registry->ecs.get<Item>(id).name == name) {
                return id;
            }
        }
        std::cout<<"Not found: "<<name<<std::endl;
        return entt::null;
    }

    void load_symbols() noexcept;



    /**
     * Base loading
     */

    inline std::string getTexturePath(std::string relative_path) noexcept {
        return getBasePath() + "/assets/textures/" + relative_path;
    }

    inline glm::vec2 extract_vec2(nlohmann::json raw) noexcept {
        return {raw["x"].get<float>(), raw["y"].get<float>()};
    }

    inline Price extract_price(nlohmann::json raw) noexcept {
        Price p;
        p.buy = extract_bignumber(raw["buy"]);
        p.sell = extract_bignumber(raw["sell"]);
        return p;
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
            info.text = Texture::init(getTexturePath("icons/notfound.png"));
        }
        return info;
    }

};
