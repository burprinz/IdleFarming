#pragma once

#include <common.h>
#include <entt/entt.hpp>

#include "utils/BigNumber.h"
#include "utils/texture.h"


/**
 * Window State
 */
constexpr int FIELDS = 0;
constexpr int GENE_LAB = 1;
constexpr int MARKETPLACE = 2;

/**
 * Field State
 */
constexpr int PLANT_FIELD = 0;
constexpr int FLOWER_FIELD = 1;
constexpr int TREE_FIELD = 2;


struct Viewport{
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
struct FarmField {
    bool is_empty = true;
    bool marked = false;
    bool unlocked = false;
    entt::entity plant;
    float grow;
    bool harvestable = false;
};

struct Plant {
    std::string name = "Plant";
    bool unlocked = false;
    float growTime = 1.0;
    int min_collect = 1;
    int max_collect = 1;
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

/**
 * Textures
 */
struct TextureInformation {
    Texture text;
    glm::vec2 base_size;
};

struct IconTexture {
    TextureInformation info;
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

    // Orders
    std::vector<entt::entity> plant_order;
    std::vector<entt::entity> flower_order;
    std::vector<entt::entity> tree_order;

    /**
     * Farm
     */
    int current_selected_field = PLANT_FIELD;

    // Plant
    entt::entity last_selected_plant = entt::null;
    entt::entity current_selected_plant = entt::null;
    // Tree
    entt::entity last_selected_tree = entt::null;
    entt::entity current_selected_tree = entt::null;
    // Flower
    entt::entity last_selected_flower = entt::null;
    entt::entity current_selected_flower = entt::null;

    /**
     * Marketplace
     */
    BigNumber market_multiplier = BigNumber(1); // -1 = MAX
private:

    int current_window = FIELDS;
    entt::entity m_money;


};
