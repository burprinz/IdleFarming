#pragma once

#include <core/registry.h>
#include <window.h>

#include "ui.h"
#include "core/draw.h"


class RenderSystem {
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		static RenderSystem init(Window* window, Registry* registry, UISystem* m_ui, Draw* m_draw) noexcept;
		void deinit() noexcept;
		void step(const float delta) noexcept;
		void reset() noexcept;



	private:
		Registry* m_registry;
		Window* m_window;
		Draw* m_draw;
		UISystem* m_ui;

		void drawBasicTextButton(std::vector<TextButton> &buttons) noexcept;

		void drawCurrentField(std::vector<entt::entity> &field_ids, glm::vec3 color, std::vector<TextButton> &buttons, entt::entity current_selected) noexcept;

};
