#include <iostream>
#include "world.h"

#include "common.h"
#include "utils/geometry.h"

constexpr float field_size = 0.5;

WorldSystem WorldSystem::init(Window *window, Registry *registry, AudioSystem *audio_engine) noexcept {
    WorldSystem self;

    self.m_window = window;
    self.m_registry = registry;

    self.m_audio_engine = audio_engine;

    self.m_reset = true;


    return self;
}

void WorldSystem::deinit() noexcept {
}

void WorldSystem::reset() noexcept {
    m_reset = false;

    create_field(m_registry->plant_field_tiles, false);
    create_field(m_registry->flower_field_tiles, false);
    create_field(m_registry->tree_field_tiles, true);
}

void WorldSystem::step(const float delta) noexcept {
    std::string title = fmt::format("{} - FPS: {:.2f} ({:.2f} ms)", PROJECT_NAME, 1 / delta, 1000 * delta);
    m_window->setTitle(title.c_str());
}


void WorldSystem::create_field(std::vector<entt::entity> &type, bool is_tree_field) noexcept {
    type.clear();

    for (int y = 0; y < 15; y++) {
        for (int x = 0; x < 10; x++) {
            entt::entity fID = m_registry->ecs.create();

            FarmField f;
            f.unlocked = true;
            f.is_tree_field = is_tree_field;
            m_registry->ecs.emplace<FarmField>(fID, f);

            glm::vec2 pos = {0.5 + x*field_size, 0.5 + y*field_size};
            m_registry->ecs.emplace<Position>(fID, pos);
            glm::vec2 size = {field_size, field_size};
            m_registry->ecs.emplace<Dimension>(fID, size);

            type.push_back(fID);
        }
    }
}
