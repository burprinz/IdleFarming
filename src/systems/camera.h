#pragma once

#include <core/registry.h>

constexpr float CAMERA_SMOOTHING_FACTOR = 3.0f;

class CameraSystem {
    public:
        static CameraSystem init(Registry* registry) noexcept;

        void step(const float delta);
        void reset() noexcept;

    private:
        Registry* m_registry;
};