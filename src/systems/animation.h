#pragma once
#include "ui.h"
#include "window.h"
#include "core/settings.h"
//
// Created by luisb on 23.09.2026.
//

struct TextureMoveAnimation {
    entt::entity texture;
    glm::vec2 start;
    glm::vec2 end;
    glm::vec2 path;
    float progress = 0.0f;
    float speed = 5.0f;
    bool is_finished = false;
};

class AnimationSystem {
public:
    AnimationSystem() = default;
    ~AnimationSystem() = default;

    static AnimationSystem init(Window *window, Registry *registry) noexcept;

    void deinit() noexcept;
    void reset() noexcept;

    void step(const float delta) noexcept;

    std::vector<std::vector<TextureMoveAnimation>> m_field_item_move_animations;
    inline void add_field_move_animation(size_t id, TextureMoveAnimation anim) noexcept {
        if (SettingsManager::instance().farmed_plants_animation_enabled && m_registry->current_selected_field == id) {
            anim.path = anim.end - anim.start;
            m_field_item_move_animations[id].push_back(anim);
        }
    }
private:
    Registry *m_registry;
    Window *m_window;
    UISystem *m_ui;

};