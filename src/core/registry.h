#pragma once

#include <common.h>
#include <entt/entt.hpp>

#include "utils/BigNumber.h"
#include "utils/texture.h"


/**
 * Window State
 */
constexpr int FIELDS = 0;
constexpr int MARKETPLACE = 1;
constexpr int TOOLSMITH = 2;
constexpr int HELPERS = 3;
constexpr int ACHIEVEMENTS = 4;
constexpr int GAMBLING = 5;
constexpr int SKILLTREE = 6;

/**
 * Field State
 */
constexpr int PLANT_FIELD = 0;
constexpr int FLOWER_FIELD = 1;
constexpr int TREE_FIELD = 2;
constexpr int CAVE_MACHINES = 3;

constexpr int FIELD_FARMING = 0;
constexpr int FIELD_HELPER_CONFIGURATION = 1;

/**
 * Plant Type for Field
 */
constexpr int NORMAL_PLANT = 0;
constexpr int COLLECT_PLANT = 1;
constexpr int INVENTORY_PLANT = 2;

/**
 * Tooltypes
 */
constexpr int PLANTING = 0;
constexpr int HARVESTING = 1;
constexpr int COLLECTING = 2;

/**
 * Card Types
 */
constexpr int CLUBS = 0;
constexpr int DIAMONDS = 1;
constexpr int HEARTS = 2;
constexpr int SPADES = 3;

/**
 * Gamble Modes
 */
constexpr int BLACKJACK = 0;

constexpr int BLACKJACK_BETTING = 0;
constexpr int BLACKJACK_TAKING = 1;
constexpr int BLACKJACK_FOLD = 2;
constexpr int BLACKJACK_END_SCREEN = 3;
constexpr int BLACKJACK_OVER = 4;


constexpr int BLACKJACK_NOTHING = 0;
constexpr int BLACKJACK_HIT = 1;
constexpr int BLACKJACK_DOUBLE_DOWN = 2;
constexpr int BLACKJACK_STAND = 3;

struct Viewport {
    glm::uvec2 m_origin;
    glm::uvec2 m_size;
};

struct CameraState {
    glm::vec2 center{0.f, 0.f};
    glm::vec2 viewSize{1.f, 1.f};

    glm::vec2 worldMin{0.f, 0.f};
    glm::vec2 worldMax{0.f, 0.f};

    float bufferLeft   = 0.f;
    float bufferRight  = 0.f;
    float bufferTop    = 0.f;
    float bufferBottom = 0.f;
};

struct Position {
    glm::vec2 pos;
};

struct Dimension {
    glm::vec2 dim;
};

struct Rect {
    glm::vec2 pos;
    glm::vec2 size;
};

struct BoundingBox {
    glm::vec2 min;
    glm::vec2 max;
};

struct PolygonShape {
    std::vector<glm::vec2> vertices;
    std::vector<int> triangle_indices;   // Triangulated indices (3 ints per triangle)
    BoundingBox bounding_box;       // Axis-aligned bounding box
};

/**
 * Field
 */
struct Field {
    std::string name; // ID Name; unique for every field
    std::string display_name; // Name for buttons
    std::string type; // Normal field, tree field, normal cave,...
    int plant_type = 0; // Normal, Collect, Inventory
    size_t list_id; // The id for the farmfields
};

struct FarmField {
    // Pos
    size_t x = -1;
    size_t y = -1;

    // Stats
    bool is_empty = true;
    bool marked = false;
    bool unlocked = false;
    entt::entity plant = entt::null;
    float grow = 0.0f;
    bool harvestable = false;

    // Unique for trees
    bool is_regrowable_field = false;

    // Helpers
    bool helper_active = false;
};

struct Plant {
    bool unlocked = false;
    float growTime = 1.0;
    int min_collect = 1;
    int max_collect = 1;

    // Unique for trees and machines
    bool is_regrowable = false;
    float regrowTime = 1.0;

    // Unique for machines
    bool has_inventory = false;
    BigNumber inventory_size;
};

struct FarmHelper {
    std::string name;
    int amount_fields = 10; // How many Fields the Farmer can handle

    int level = 0; // Level -> Cost for upgrade = BuyPrice * (2^level)
    bool max_reached = false;

    entt::entity field = entt::null; // The ID of the field the person is working on

    std::vector<entt::entity> working_area; // Selected Working Fields
    std::vector<entt::entity> working_plants; // Wanted Plants for selected Fields
};


/**
 * Gamble
 */
struct PlayingCard {
    std::string name;
    int symbol = -1;
    int value = -1;
    bool hidden = false;
};


struct Color {
    glm::vec3 c;
};

struct Amount {
    BigNumber am;
};

struct Price {
    BigNumber buy;
    BigNumber sell;
};

struct Unlockable {
    bool unlocked = false;
    BigNumber price;
};

struct Inventory {
    BigNumber size;
    BigNumber amount;
};

/**
 * Items
 */

struct ItemCategory {
    std::string id_name;
    std::string display_name;
    std::vector<entt::entity> items;
};

struct Item {
    std::string name = "plant";
    std::string display_name = "Plant";
    std::vector<entt::entity> categories;
};

struct Tool {
    std::string field_type;
    std::string work_area = "???"; // Only for tool description ingame
    int usage; // PLANTING, HARVESTING, COLLECTING
    int stage; // eg. 1=iron, 2=gold,...
    int radius; // Working radius
};

struct Achievement {
    std::string name = "???";
    std::string description = "Do ...";
    std::vector<std::string> needed_stat_types;
    BigNumber amount;
    bool done = false;
};

struct Skill {
    std::string name = "???";
    std::string description = "Unlocks ...";
    entt::entity parent = entt::null;
    glm::vec2 pos;
};

struct BuyAmount {
    std::vector<entt::entity> item_ids;
    std::vector<BigNumber> amounts;
};


/**
 * Textures
 */
struct TextureInformation {
    Texture text;
    glm::vec2 base_size;
};

struct IconTexture {
    TextureInformation info;
    TextureInformation ungrownInfo;
    TextureInformation icon;
};

// NOTE: For your own project you may want to move the registry to a different file, as it grows in size.
//	 Depending on the requirements of your projects it may also make sense to have multiple registries.
class Registry {
public:
    static Registry *init() noexcept;

    Viewport m_viewport;
    CameraState m_camera;

    bool debug_mode = false;

    void on_key_callback(int key, int action) noexcept;
    void on_mouse_callback(int key, int action) noexcept;

    inline entt::entity money() noexcept { return m_money; }

    inline entt::entity create_money() noexcept {
        m_money = ecs.create();
        return m_money;
    }

    inline void change_window(int window_id) noexcept {
        current_window = window_id;
    }

    inline int window() noexcept {
        return current_window;
    }

    // Entity Component System
    entt::registry ecs;

    // Which keys are pressed
    std::map<std::string, bool> keys;

    // Camera information
    glm::vec2 camera_origin;
    glm::vec2 camera_pos = {0,0};

    // Debug-Mode (F5)
    bool debug = false;

    /**
     * Textures
     */
    entt::entity lock_locked_texture;
    entt::entity lock_unlocked_texture;

    /**
     * Farm
     */
    int field_mode = FIELD_FARMING;
    std::vector<entt::entity> field_order;
    std::vector<Color> field_colors;
    size_t current_selected_field = 0;
    std::vector<std::vector<entt::entity>> plant_order;
    std::vector<entt::entity> last_selected_plant;
    std::vector<entt::entity> current_selected_plant;
    std::vector<std::vector<std::vector<entt::entity>>> plant_field_tiles;

    /**
     * Toolsmith
     */
    entt::entity current_hover_tool;
    std::vector<entt::entity> tool_order;
    std::map<std::string, std::string> tool_work_area_names;
    std::map<std::string, entt::entity> current_plant_tools;
    std::map<std::string, entt::entity> current_harvest_tools;
    std::map<std::string, entt::entity> current_collect_tools;

    /**
     * Marketplace
     */
    std::vector<entt::entity> market_category_order;
    std::vector<entt::entity> market_item_order;
    entt::entity current_market_category;
    BigNumber market_multiplier = BigNumber(1); // -1 = MAX

    /**
     * Helpers
     */
    std::vector<entt::entity> helpers_order;
    std::map<entt::entity, entt::entity> field_helper; // FieldID, HelperID


    /**
     * Gamble
     */

    float animation_value = 0.0f;

    entt::entity chips;
    entt::entity back_card;
    std::vector<entt::entity> card_list;
    std::vector<int> chip_list;
    std::string winner_name = "???";

    int gamble_mode = -1;
    int gamble_stage = -1;
    int bet_multiplicator = 1;
    int bet_amount = 0;
    int max_bet_amount = 10000;
    std::vector<int> bet_chips;
    bool can_click = true;
    int blackjack_action = BLACKJACK_NOTHING;

    // Blackjack
    std::vector<entt::entity> player_cards;
    std::vector<entt::entity> dealer_cards;
    int player_value = 0;
    int dealer_value = 0;

    /**
     * Achievements
     */
    std::vector<entt::entity> achievements_order;

    /**
     * Skilltree
     */
    glm::vec2 skilltree_pos_offset = {0.f, 0.f};


    /**
     * Buying
     */
    entt::entity trying_to_buy = entt::null;


    /**
     * Maps
     */
    std::unordered_map<std::string, entt::entity> item_entities;
    std::unordered_map<std::string, entt::entity> tool_entities;
    std::unordered_map<std::string, entt::entity> helper_entities;
    std::unordered_map<std::string, entt::entity> achievement_entities;
    std::unordered_map<std::string, size_t> field_ids;
    std::unordered_map<std::string, entt::entity> playing_cards;
    std::unordered_map<int, entt::entity> poker_chips;

private:

    int current_window = FIELDS;
    entt::entity m_money;


};
