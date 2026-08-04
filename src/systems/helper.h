//
// Created by luisb on 04.08.2026.
//

#include "window.h"
#include "core/registry.h"

class HelperSystem {
public:
    static HelperSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;

private:

    Registry* m_registry;
    Window* m_window;

};
