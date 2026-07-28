#pragma once

#include <core/registry.h>
#include <window.h>

#include "ui.h"
#include "utils/draw.h"


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

		void drawBasicTextButtons(std::vector<TextButton> &buttons) noexcept;

		void drawCurrentField(std::vector<std::vector<entt::entity>> &field_ids, glm::vec3 color, std::vector<TextButton> &buttons, entt::entity current_selected) noexcept;

		glm::vec2 getPopupPositionForMouse(glm::vec2 popup_size) noexcept;
		void drawPopup(std::string msg, glm::vec2 pos, glm::vec2 size, glm::vec3 text_color=white) noexcept;


		inline void drawLockInBlackQuad(glm::vec2 pos, glm::vec2 size, bool open_lock) {
			// Not unlocked
			m_draw->fillRectBySize(pos, size, black, 1.0f);
			Texture lock_text;
			if (open_lock) {
				lock_text = m_registry->ecs.get<TextureInformation>(m_registry->lock_unlocked_texture).text;
			} else {
				lock_text = m_registry->ecs.get<TextureInformation>(m_registry->lock_locked_texture).text;
			}
			m_draw->drawTexture(lock_text, pos, glm::vec2{size.y, size.y} / 2.2f);
		}

};
