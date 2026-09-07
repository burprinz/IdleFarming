//
// Created by luisb on 01.09.2026.
//

#include "ui.h"
#include "window.h"
#include "core/registry.h"
#include "helpers/sizes.h"

class SkilltreeSystem {
public:
    static SkilltreeSystem init(Window *window, Registry *registry, UISystem *ui) noexcept;

    void deinit() noexcept;

    void reset() noexcept;
    void init_buttons() noexcept;
    bool left_click() noexcept;

    void step(const float delta) noexcept;


private:
    Registry *m_registry;
    Window *m_window;
    UISystem *m_ui;

    bool right_click_pressed = false;
    glm::vec2 origin_mouse = {0.0, 0.0};
    glm::vec2 old_offset = {0.0, 0.0};

    inline entt::entity create_skill(glm::vec2 pos, std::string name, std::string description, std::function<void()> check_func, int levels, entt::entity parent=entt::null) noexcept {

        Registry *registry = m_registry;

        entt::entity id = m_registry->ecs.create();
        Skill skill;
        skill.name = name;
        skill.description = description;
        skill.parent = parent;
        skill.pos = pos;
        skill.level = 0;
        skill.max_level = levels;
        m_registry->ecs.emplace<Skill>(id, skill);

        Unlockable skill_unlock;
        skill_unlock.unlocked = false;
        m_registry->ecs.emplace<Unlockable>(id, skill_unlock);

        TextButton skill_button;
        skill_button.pos = pos;
        skill_button.size = skill_button_size;
        skill_button.id_label = id;
        skill_button.on_click = [registry, id, check_func, parent] {
            if (parent != entt::null) {
                Unlockable parent_unlock = registry->ecs.get<Unlockable>(parent);
                if (!parent_unlock.unlocked) {
                    return;
                }
            }

            Skill &s = registry->ecs.get<Skill>(id);
            if (s.level < s.max_level) {
                s.level++;
                check_func();

                if (s.level == s.max_level) {
                    Unlockable &skill_unlock = registry->ecs.get<Unlockable>(id);
                    skill_unlock.unlocked = true;
                }
            }

        };
        m_ui->m_skilltree_buttons.push_back(skill_button);

        return id;
    }

};
