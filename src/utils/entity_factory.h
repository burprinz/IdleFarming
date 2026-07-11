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
            bool is_tree = field["is_tree"].get<bool>();
            bool has_inv = field["has_inv"].get<bool>();
            load_field_plants(field_name, is_tree, has_inv);
        }
    }

    inline void load_field_plants(std::string field_name, bool is_tree, bool has_inv) noexcept {
        std::string json_path = "/assets/data/basevalues/field_plants/" + field_name + ".json";
        nlohmann::json plant_list_json;
        bool loaded = readPath(json_path, &plant_list_json);
        if (!loaded) {
            std::cout<<"Error loading Field List"<<std::endl;
        }

        // The field order for the field selection
        m_registry->field_order.push_back(plant_list_json["name"]);

        // The shown field color
        m_registry->field_colors.push_back(extract_color(plant_list_json["color"]));

        // Every order the plants should be shown as buttons
        std::vector<entt::entity> plant_order;

        for (auto& plant : plant_list_json["plants"]) {
            std::string plant_name = plant.get<std::string>();
            std::string plant_path = "/assets/data/basevalues/plantables/" + field_name + "/" + plant_name + ".json";

            nlohmann::json plant_json;
            loaded = readPath(plant_path, &plant_json);
            if (!loaded) {
                std::cout<<"Error loading " << plant_name << " in EntityFactory with path <"<<plant_path<<">"<<std::endl;
                continue;
            }
            create_plant_like(plant_json, plant_order, is_tree, has_inv);
        }

        m_registry->tmp_order.push_back(plant_order);
        m_registry->last_selected_tmp.push_back(entt::null);
        m_registry->current_selected_tmp.push_back(entt::null);

        std::vector<entt::entity> field_tiles;
        create_field(field_tiles, is_tree || has_inv);
        m_registry->tmp_field_tiles.push_back(field_tiles);
    }

    inline void create_field(std::vector<entt::entity> &type, bool is_regrowable) noexcept {
        type.clear();

        for (int y = 0; y < 15; y++) {
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

                type.push_back(fID);
            }
        }
    }

    void create_plant_like(nlohmann::json plant, std::vector<entt::entity> &order, bool is_tree, bool has_inv) noexcept;

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
