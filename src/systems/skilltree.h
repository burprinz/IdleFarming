//
// Created by luisb on 01.09.2026.
//

#include "ui.h"
#include "window.h"
#include "core/lang.h"
#include "core/multiplier.h"
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

    inline std::vector<std::string> create_lines(std::string desc, std::vector<std::string> effects) {
        std::vector<std::string> result;
        result.push_back(desc);
        if (effects.size() > 0 ) {
            result.push_back("Current Effects)");
            for (std::string effect : effects) {
                result.push_back(Language::instance().multiplier()[effect] + " x" + std::to_string(Multiplier::instance().get_stat(effect)));
            }
        }
        return result;
    }


    inline entt::entity create_skill(glm::vec2 pos, std::string id_name, std::string name, std::string description, std::vector<std::string> effects, std::function<void(entt::entity)> check_func, int levels, entt::entity parent=entt::null) noexcept {

        Registry *registry = m_registry;

        entt::entity id = m_registry->skilltree_entities[id_name];;
        Skill &skill = m_registry->ecs.get<Skill>(id);
        skill.name = name;
        skill.description = description;
        skill.parent = parent;
        skill.pos = pos;
        skill.max_level = levels;
        skill.display_lines = create_lines(description, effects);



        TextButton skill_button;
        skill_button.pos = pos;
        skill_button.size = skill_button_size;
        skill_button.id_label = id;
        skill_button.on_click = [this, registry, id, check_func, parent, description, effects] {
            if (parent != entt::null) {
                Skill parent_skill = registry->ecs.get<Skill>(parent);
                //Unlockable parent_unlock = registry->ecs.get<Unlockable>(parent);
                if (parent_skill.level <= 0) {
                    return;
                }
            }

            Skill &s = registry->ecs.get<Skill>(id);
            if (s.level < s.max_level) {
                s.level++;
                check_func(id);
                s.display_lines = create_lines(description, effects);


                if (s.level == s.max_level) {
                    Unlockable &skill_unlock = registry->ecs.get<Unlockable>(id);
                    skill_unlock.unlocked = true;
                }
            }

        };
        m_ui->m_skilltree_buttons.push_back(skill_button);


        if (skill.level > 0) {
            check_func(id);
            skill.display_lines = create_lines(description, effects);
        }

        return id;
    }

};
