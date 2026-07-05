#pragma once

#include <core/registry.h>
#include <window.h>
#include <systems/audio.h>


class WorldSystem {
	public:
		static WorldSystem init(Window* window, Registry* registry, AudioSystem* audio_engine) noexcept;
		void deinit() noexcept;

		void step(const float delta) noexcept;
		void reset() noexcept;

		inline bool shouldReset() noexcept { return m_reset; }
		inline void requestReset() noexcept { m_reset = true; }

	private:
		Window* m_window;
		Registry* m_registry;
		AudioSystem* m_audio_engine;

		bool m_reset;

		void create_field(std::vector<entt::entity> &type, bool is_regrowable) noexcept;



};

