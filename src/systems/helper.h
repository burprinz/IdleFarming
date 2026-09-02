//
// Created by luisb on 04.08.2026.
//

#include "ui.h"
#include "window.h"
#include "core/registry.h"

class HelperSystem {
public:
    static HelperSystem init(Window* window, Registry* registry, UISystem* ui) noexcept;
    void deinit() noexcept;
    void reset() noexcept;
    bool left_click() noexcept;

    void step(const float delta) noexcept;


private:

    Registry* m_registry;
    Window* m_window;
    UISystem* m_ui;

    void initButtons() noexcept;

};
