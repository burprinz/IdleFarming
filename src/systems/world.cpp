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
}

void WorldSystem::step(const float delta) noexcept {
    std::string title = fmt::format("{} - FPS: {:.2f} ({:.2f} ms)", PROJECT_NAME, 1 / delta, 1000 * delta);
    m_window->setTitle(title.c_str());
}
