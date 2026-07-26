#pragma once

// Default constant values
namespace config {

    constexpr float GAME_ASPECT_RATIO = 16.f / 9.f; // Aspect ratio of the in-game map
	constexpr int MAX_BEAM = 32;
    
    // Renamed Camera View from window.h
    constexpr float CAMERA_VIEW_WIDTH  = 16.f; //Formerly WINDOW_X_EXTENT
    constexpr float CAMERA_VIEW_HEIGHT = 9.f; //Formerly WINDOW_Y_EXTENT
    constexpr float SCREEN_DARKNESS = 1; // Default screen darkness
	constexpr float PHYSICS_TIME_STEP = 1.f / 60.f; // Physics time step for fixed update loop

	//Beam Values
	constexpr float BEAM_HALF_WIDTH = 0.02f; // total width = 0.04 in world units
	constexpr float BEAM_CORE_RATIO = 0.1f;

    constexpr float INF = std::numeric_limits<float>::infinity();
    constexpr float EPSILON = 0.0001f;
	constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
	constexpr float FLOAT_MIN = std::numeric_limits<float>::min();
}