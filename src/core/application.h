#pragma once

#include <common.h>
#include <./utils/command_line_options.h>

#include <./systems/systems.h>

#include <./window.h>

#include "discord.h"
#include "../utils/draw.h"
#include "systems/field.h"
#include "systems/unlock.h"
#include "data/entity_factory.h"
#include "systems/achievement.h"
#include "systems/gamble.h"
#include "systems/helper.h"
#include "systems/marketplace.h"
#include "systems/skilltree.h"
#include "systems/toolsmith.h"


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
		ToolSystem m_tools;
		MarketSystem m_market;

		HelperSystem m_helper;

		UnlockSystem m_unlock;
		AchievementSystem m_achievement;

		GambleSystem m_gamble;
		SkilltreeSystem m_skilltree;

		EntityFactory m_entity_factory;

		Registry* m_registry;
		Discord m_discord;

		void reset() noexcept;

		void changeWindow() noexcept;

		void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;
        void onMouseCallback(GLFWwindow* window, int button, int action, int mods) noexcept;
		void onMouseClickCallback(GLFWwindow* window, int button, int action, int mods) noexcept;
        void onResizeCallback(GLFWwindow* window, int width, int height) noexcept;
		void onScrollCallback(GLFWwindow* window, double xoffset, double yoffset) noexcept;

		inline void tests() noexcept {
			test_offline_stats_progress();
		}

		inline void test_offline_stats_progress() noexcept {
			std::cout<<"TEST START -------- Offline Progress Stats Test"<<std::endl;
			BigNumber prev_plant = StatsManager::instance().get("auto_planted");
			BigNumber prev_harvest = StatsManager::instance().get("auto_harvested");
			BigNumber prev_collect = StatsManager::instance().get("auto_collected");
			StatsManager::instance().set("auto_planted", 0);
			StatsManager::instance().set("auto_harvested", 0);
			StatsManager::instance().set("auto_collected", 0);

			std::cout<<"1) Values:"<<
				" auto_planted="<<StatsManager::instance().get("auto_planted").toString()<<
				" auto_harvested="<<StatsManager::instance().get("auto_harvested").toString()<<
					" auto_collected="<<StatsManager::instance().get("auto_collected").toString()<<std::endl;

			std::cout<<"2) Applying 100sec Helper Growtime for Single apple"<<std::endl;
			entt::entity fieldID = m_registry->plant_field_tiles[2][0][0];
			entt::entity plantID = m_registry->item_entities["apple"];
			entt::entity helperID = m_registry->helper_entities["Hinata"];

			FarmField &f = m_registry->ecs.get<FarmField>(fieldID);
			FarmHelper &helper = m_registry->ecs.get<FarmHelper>(helperID);
			plant_tile(m_registry, fieldID, plantID, true);
			f.helper_active = true;
			helper.working_area.push_back(fieldID);
			helper.working_plants.push_back(plantID);
			apply_helper_growtime(m_registry, fieldID, 100);

			std::cout<<"3) Values:"<<
				" auto_planted="<<StatsManager::instance().get("auto_planted").toString()<<
				" auto_harvested="<<StatsManager::instance().get("auto_harvested").toString()<<
					" auto_collected="<<StatsManager::instance().get("auto_collected").toString()<<std::endl;


			StatsManager::instance().set("auto_planted", prev_plant);
			StatsManager::instance().set("auto_harvested", prev_harvest);
			StatsManager::instance().set("auto_collected", prev_collect);
			std::cout<<"TEST END -------- Offline Progress Stats Test"<<std::endl;
		}


};
