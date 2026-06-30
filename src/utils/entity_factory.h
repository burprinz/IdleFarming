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
    void createFromFile(const std::string& path) noexcept;

    inline void loadBaseValues() noexcept {
        load_base_plants();
        load_base_flowers();
    }


private:
    Registry* m_registry;
    Window* m_window;

    std::string m_texture_base_path = getBasePath() + "/assets/textures/";

    nlohmann::json m_json;

    nlohmann::json m_levelJson;
    nlohmann::json m_objectJson;


    void create_plant_like(nlohmann::json plant, std::vector<entt::entity> &order) noexcept;



    /**
     * Base loading
     */
    inline void load_base_plants() noexcept {
        std::string json_path = "/assets/data/basevalues/plants.json";
        bool loaded = readPath(json_path, &m_json);
        if (!loaded) {
            std::cout<<"Error loading base plants in EntityFactory"<<std::endl;
        }

        for (auto& plant : m_json["plants"]) {
            create_plant_like(plant, m_registry->plant_order);
        }
        std::cout<<"Plants loaded"<<std::endl;
    }

    inline void load_base_flowers() noexcept {
        std::string json_path = "/assets/data/basevalues/flowers.json";
        bool loaded = readPath(json_path, &m_json);
        if (!loaded) {
            std::cout<<"Error loading base flowers in EntityFactory"<<std::endl;
        }
        for (auto& flower : m_json["flowers"]) {
            create_plant_like(flower, m_registry->flower_order);
        }
        std::cout<<"Flowers loaded"<<std::endl;
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
