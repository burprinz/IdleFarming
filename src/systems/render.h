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

		void drawField() noexcept;
		void drawMarketplace() noexcept;
		void drawToolsmith() noexcept;
		void drawHelpers() noexcept;
		void drawAchievements() noexcept;
		void drawGambling() noexcept;


		void drawBlackjack() noexcept;

		void drawBasicTextButtons(std::vector<TextButton> &buttons, glm::vec3 text_color, bool with_max_size) noexcept;
		void drawBasicTextButton(TextButton &b, glm::vec3 text_color, bool with_max_size) noexcept;

		void drawCurrentField(std::vector<std::vector<entt::entity>> &field_ids, glm::vec3 color, std::vector<TextButton> &buttons, entt::entity current_selected, bool for_helper) noexcept;

		//glm::vec2 getPopupPositionForMouse(glm::vec2 popup_size) noexcept;
		//void drawPopup(std::string msg, glm::vec2 pos, glm::vec2 size, glm::vec3 text_color=white) noexcept;


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

	inline void drawPopup(std::string title, std::string description) noexcept {
			m_draw->fillRoundRect(message_popup_center, message_popup_size,0.01f, black, 1.0f);
			m_draw->drawStringInBox(title, message_popup_title_center, message_popup_title_size, white);
			float x = message_popup_title_center.x - message_popup_title_size.x / 2.0f + normal_gap;
			float y = message_popup_title_center.y - message_popup_title_size.y / 2.0f + small_gap;
			m_draw->drawLine({x, y}, {x+message_popup_title_size.x-2*normal_gap, y}, white);
			m_draw->drawStringInBox(description, message_popup_description_center, message_popup_description_size, white);
			drawBasicTextButtons(m_ui->message_popup_close_buttons, white, true);
			m_draw->drawRoundRect(message_popup_center, message_popup_size, 0.01f, white);
	}


};
