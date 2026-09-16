//
// Created by luisb on 16.09.2026.
//

#include "entity_factory.h"

    void EntityFactory::save_time() noexcept {
        nlohmann::json j;
        j["time"] = get_current_timestamp();
        save("save_time.json", j);

    }

    long long EntityFactory::load_time_difference() noexcept {
        long long start_time = load_time();
        long long current_time = get_current_timestamp();
        return current_time - start_time;
    }

    long long EntityFactory::load_time() noexcept {
        std::string json_path = save_path + "save_time.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading money"<<std::endl;
            return get_current_timestamp();
        }

        return json["time"].get<long long>();
    }

    void EntityFactory::save_money() noexcept {
        BigNumber money = m_registry->ecs.get<Amount>(m_registry->money()).am;
        nlohmann::json result = {{"money", create_bignumber_json(money)}};
        save("money.json", result);
    }

    void EntityFactory::load_money() noexcept {
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

    void EntityFactory::save_item_amounts() noexcept {
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

    void EntityFactory::load_item_amounts() noexcept {
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

    void EntityFactory::save_tools() noexcept {
        nlohmann::json unlocked_tools = create_unlocked_tools_json(m_registry);
        nlohmann::json activated_tools = create_activated_tools_json(m_registry);

        nlohmann::json result = {{"unlocked", unlocked_tools}, {"activated", activated_tools}};
        save("tools.json", result);
    }

    void EntityFactory::load_tools() noexcept {
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

    void EntityFactory::save_fields() noexcept {

        std::vector<nlohmann::json> fields;

        for (size_t i = 0; i < m_registry->field_order.size(); i++) {
            entt::entity field_id = m_registry->field_order[i];
            fields.push_back(create_field_json(m_registry, i, field_id));
        }

        nlohmann::json result;
        result["fields"] = fields;

        save("fields.json", result);

    }

    void EntityFactory::load_fields(int time_diff) noexcept {
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

    void EntityFactory::save_helpers() noexcept {
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

    void EntityFactory::load_helpers(int time_diff) noexcept {
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

    void EntityFactory::save_unlocked_plants() noexcept {
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

    void EntityFactory::load_unlocked_plants() noexcept {
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

    void EntityFactory::save_stats() noexcept {
        nlohmann::json result = create_stats_json();
        save("stats.json", result);
    }

    void EntityFactory::load_stats() noexcept {
        std::string json_path = save_path + "stats.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading stats"<<std::endl;
            return;
        }
        load_current_stats(json);
    }

    void EntityFactory::save_done_achievements() noexcept {
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

    void EntityFactory::load_done_achievements() noexcept {
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

    void EntityFactory::save_skilltree_progress() noexcept {

        std::vector<nlohmann::json> skills;

        for (entt::entity skill_id : m_registry->ecs.view<Skill>()) {
            Skill s = m_registry->ecs.get<Skill>(skill_id);
            if (s.level > 0) {
                skills.push_back(create_skill_json(m_registry, skill_id));
            }
        }

        nlohmann::json result;
        result["skills"] = skills;

        save("skills.json", result);
    }

    void EntityFactory::load_skilltree_progress() noexcept {
        std::string json_path = save_path + "skills.json";
        nlohmann::json json;
        bool loaded = read(json_path, &json);
        if (!loaded) {
            std::cout<<"Error loading skill progress"<<std::endl;
            return;
        }

        // TODO Prüfen, ob stats auch bei offline time verwendet werden
        for (const auto& skill : json["skills"]) {
            load_skill_json(m_registry, skill);
        }
    }