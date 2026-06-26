#pragma once

#include <common.h>

namespace assets {

enum struct Sound {
	music,
	count
};

enum struct Mesh {
	player,
	light,
	count
};

enum struct Shader {
	player,
	sprite,
	wind,
	egg,
    line,
	platform,
	illumination,
	beam_illumination,
	animation,
	text,
	shadow,
	circle,
	polygon,
	beam,
	count
};

enum struct Texture {
		count
};


template<typename AssetType>
std::string getAssetPath(const AssetType asset_id) noexcept;
} // namespace assets