//
// Created by luisb on 23.06.2026.
//

#include "window.h"
#include "core/registry.h"

class FieldSystem {
public:
    static FieldSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;

private:

    Registry* m_registry;
    Window* m_window;

    void try_to_plant(std::vector<std::vector<entt::entity>> field, entt::entity p_id) noexcept;


};
