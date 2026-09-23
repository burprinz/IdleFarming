//
// Created by luisb on 23.06.2026.
//

#include "animation.h"
#include "ui.h"
#include "window.h"
#include "core/registry.h"

class FieldSystem {
public:
    static FieldSystem init(Window* window, Registry* registry, UISystem* ui, AnimationSystem* animation) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;

    bool left_click() noexcept;

private:

    Registry* m_registry;
    Window* m_window;
    UISystem* m_ui;
    AnimationSystem* m_animation;

    void initButtons() noexcept;
    void initPlantLikePlantSelectionButtons(std::vector<entt::entity> order, entt::entity &last_selected, entt::entity &current_selected, std::vector<TextButton> &buttons) noexcept;

    void try_to_plant(std::vector<std::vector<entt::entity>> field, entt::entity p_id) noexcept;


};
