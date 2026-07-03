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
        load_base_plant_like("plants", m_registry->plant_order, false);
        load_base_plant_like("flowers", m_registry->flower_order, false);
        load_base_plant_like("trees", m_registry->tree_order, true);
    }


private:
    Registry* m_registry;
    Window* m_window;

    std::string m_texture_base_path = getBasePath() + "/assets/textures/";

    nlohmann::json m_json;

    nlohmann::json m_levelJson;
    nlohmann::json m_objectJson;

    void create_plant_like(nlohmann::json plant, std::vector<entt::entity> &order, bool is_tree) noexcept;


    /**
     * Base loading
     */

    inline void load_base_plant_like(std::string type, std::vector<entt::entity> &order, bool is_tree) {
        order.clear();

        std::string json_path = "/assets/data/basevalues/" + type + ".json";
        bool loaded = readPath(json_path, &m_json);
        if (!loaded) {
            std::cout<<"Error loading base plants in EntityFactory"<<std::endl;
        }

        for (auto& plant : m_json[type]) {
            std::string plant_name = plant.get<std::string>();
            std::string plant_path = "/assets/data/basevalues/plantables/" + type + "/" + plant_name + ".json";
            nlohmann::json plant_json;
            loaded = readPath(plant_path, &plant_json);
            if (!loaded) {
                std::cout<<"Error loading " << plant_name << " in EntityFactory with path <"<<plant_path<<">"<<std::endl;
                continue;
            }
            create_plant_like(plant_json, order, is_tree);
        }
        std::cout<<"Plants loaded"<<std::endl;
    }

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

};
