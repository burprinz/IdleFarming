//
// Created by luisb on 05.08.2026.
//


/**
 * For Save-Game Loading
 */


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