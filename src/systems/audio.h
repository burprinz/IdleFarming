#pragma once

#include <common.h>
#include <assets.h>
#include <miniaudio.h>

class AudioSystem {
	public:
		AudioSystem() = default;
		~AudioSystem() = default;

		static AudioSystem init() noexcept;
		void deinit() noexcept;

		inline ma_engine* engine() noexcept { return m_engine; }

		inline ma_sound* backgroundMusic() noexcept { return m_background_music; }
        
		inline void set_music_volume(float volume) noexcept {ma_sound_set_volume(m_background_music, volume); }

		inline float get_music_volume() noexcept {return ma_sound_get_volume(m_background_music);}

	private:
		ma_engine* m_engine;
		ma_sound* m_background_music;

		ma_sound* loadSound(const assets::Sound asset) noexcept;
};
