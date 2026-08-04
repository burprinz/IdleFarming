#pragma once

#include <common.h>
#include <./utils/command_line_options.h>

#include <./systems/systems.h>

#include <./window.h>

#include "discord.h"
#include "../utils/draw.h"
#include "systems/field.h"
#include "systems/unlock.h"
#include "core/entity_factory.h"
#include "systems/helper.h"


class Application {
	public:
		// NOTE: You may want to use the constructor and destructor for initialization
		//       and deinitialization of objects. For the template we opted to use explicit
		//       initialization and deinitialization to avoid hidden control flow.
		static std::optional<Application> init(const CommandLineOptions& options) noexcept;
		void deinit() noexcept;
		void run() noexcept;

	private:
		Window* m_window;

		AudioSystem* m_audio_engine;
		CameraSystem m_camera;

		WorldSystem m_world;

		UISystem* m_ui;
		Draw* m_draw;
		RenderSystem m_render;

		FieldSystem m_field;
		HelperSystem m_helper;

		UnlockSystem m_unlock;

		EntityFactory m_entity_factory;

		Registry* m_registry;
		Discord m_discord;

		void reset() noexcept;

		void changeWindow() noexcept;

		void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;
        void onMouseCallback(GLFWwindow* window, int button, int action, int mods) noexcept;
		void onMouseClickCallback(GLFWwindow* window, int button, int action, int mods) noexcept;
        void onResizeCallback(GLFWwindow* window, int width, int height) noexcept;

};
