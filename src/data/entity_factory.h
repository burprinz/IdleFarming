//
// Created by luisb on 10.03.2026.
//


#include <iostream>
#include <nlohmann/adl_serializer.hpp>

#include "window.h"
#include "core/registry.h"
#include <nlohmann/json.hpp>
#include "../core/save.h"
#include "core/load.h"

class EntityFactory {
public:
    static EntityFactory init(Window* window, Registry* registry) noexcept;

    bool read(const std::string& path, nlohmann::json* json) noexcept;
    bool save(const std::string& path, nlohmann::json json) noexcept;

    inline void loadBaseValues() noexcept {

        load_item_categories();

        load_symbols();

        load_base_fields();

        create_tools();

        create_helpers();

        create_achievements();

        create_playing_cards();
        create_poker_chips();

        preload_skilltree_infos();

        load_lang("english");

        int time_diff = static_cast<int>(load_time_difference());
        std::cout<<"Offline progress time: "<<time_diff<<"s"<<std::endl;

        load_stats();
        load_unlocked_plants();
        load_money();
        load_item_amounts();
        load_tools();
        load_done_achievements();
        load_skilltree_progress();
        load_fields(time_diff);
        load_helpers(time_diff);
    }

    inline void save_values() noexcept {
        save_time();
        save_unlocked_plants();
        save_money();
        save_item_amounts();
        save_tools();
        save_fields();
        save_helpers();
        save_stats();
        save_done_achievements();
        save_skilltree_progress();
    }


private:
    Registry* m_registry;
    Window* m_window;

    std::string texture_path = getBasePath() + "/assets/textures/";
    std::string save_path = "/assets/data/save/";


    nlohmann::json m_json;

    nlohmann::json m_levelJson;
    nlohmann::json m_objectJson;

    void load_symbols() noexcept;

    void load_item_categories() noexcept;

    void load_base_fields() noexcept;

    void load_field_plants(Field *f, bool is_tree, bool has_inv, entt::entity parent_id) noexcept;

    void create_field(std::vector<std::vector<entt::entity>> &tiles, bool is_regrowable, entt::entity parent_field_id) noexcept;

    void create_plant_like(nlohmann::json plant, std::vector<entt::entity> &order, bool is_tree, bool has_inv) noexcept;

    void create_tools() noexcept;

    void load_tool_list(nlohmann::json tool_list) noexcept;

    void create_helpers() noexcept;

    void load_helper(nlohmann::json raw_helper_infos, std::string type) noexcept;

    void create_achievements() noexcept;

    void create_achievement_type(std::string name) noexcept;

    void create_playing_cards() noexcept;

    void create_poker_chips() noexcept;

    void preload_skilltree_infos() noexcept;

    void load_lang(std::string lang) noexcept;


    void save_time() noexcept;
    long long load_time_difference() noexcept;
    long long load_time() noexcept;

    void save_money() noexcept;
    void load_money() noexcept;

    void save_item_amounts() noexcept;
    void load_item_amounts() noexcept;

    void save_tools() noexcept;
    void load_tools() noexcept;

    void save_fields() noexcept;
    void load_fields(int time_diff) noexcept;

    void save_helpers() noexcept;
    void load_helpers(int time_diff) noexcept;

    void save_unlocked_plants() noexcept;
    void load_unlocked_plants() noexcept;

    void save_stats() noexcept;
    void load_stats() noexcept;

    void save_done_achievements() noexcept;
    void load_done_achievements() noexcept;

    void save_skilltree_progress() noexcept;
    void load_skilltree_progress() noexcept;
};
