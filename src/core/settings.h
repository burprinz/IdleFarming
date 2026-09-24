#pragma once

//
// Created by luisb on 24.09.2026.
//

class SettingsManager {
public:

    static SettingsManager& instance() {
        static SettingsManager instance;
        return instance;
    }

    bool farmed_plants_animation_enabled = true;
};