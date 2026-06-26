#include "audio.h"

constexpr float MUSIC_VOLUME = 0.0f;

AudioSystem AudioSystem::init() noexcept {
	AudioSystem self;

	self.m_engine = new ma_engine;
	ma_engine_init(nullptr, self.m_engine);
	self.m_background_music = self.loadSound(assets::Sound::music);
	ma_sound_set_volume(self.m_background_music, MUSIC_VOLUME);

	return self;
}

void AudioSystem::deinit() noexcept {
	ma_sound_uninit(m_background_music);
	delete m_background_music;

	ma_engine_uninit(m_engine);
	delete m_engine;
}

ma_sound* AudioSystem::loadSound(const assets::Sound asset) noexcept {
	const std::string asset_path = assets::getAssetPath(asset);
	ma_sound* sound = nullptr;
	if ((sound = (new ma_sound)) == nullptr) {
		fmt::println(stderr, "Failed to allocate sound");
		return nullptr;
	}

	ma_result result;
	if ((result = ma_sound_init_from_file(m_engine, asset_path.c_str(), 0, nullptr, nullptr, sound)) != MA_SUCCESS) {
		fmt::println(stderr, "Failed to load \"{}\": {}", asset_path, ma_result_description(result));
		delete sound;
		return nullptr;
	}

	return sound;
}
