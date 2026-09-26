//
// Created by luisb on 25.09.2026.
//

#include "entity_factory.h"


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
