//
// Created by luisb on 25.09.2026.
//

#include "entity_factory.h"
#include <fstream>
#include <iostream>

bool EntityFactory::read(const std::string& path, nlohmann::json* json_link) noexcept {
    std::string basePath = getBasePath();
    std::string fullPath = basePath + path;

    // Allows for path to be just the filename
    if (!std::ifstream(fullPath).good())
    {
        fullPath = basePath + path;
    }

    //Tries Reading File
    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        fmt::println("Could not open level file: '{}'", fullPath);
        return false;
    }

    //Tries Parsing JSON
    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const std::exception& e)
    {
        fmt::println("Failed to parse JSON from '{}': {}", fullPath, e.what());
        return false;
    }

    //Saves Layout and Path
    //CHATGPT Suggested move as it is better in terms of memory. Also in this StackOverflow: https://stackoverflow.com/questions/3413470/what-is-stdmove-and-when-should-it-be-used
    *json_link = std::move(j);

    //fmt::println("Successfully loaded level file: '{}'", fullPath);
    return true;
}

bool EntityFactory::save(const std::string &path, const nlohmann::json json) noexcept {
    std::string full_path = getBasePath() + save_path + path;

    std::ofstream datei(full_path, std::ios::trunc);
    if (!datei.is_open()) {
        std::cerr << "Fehler: Datei konnte nicht geöffnet werden: " << full_path << std::endl;
        return false;
    }
    try {
        datei << json.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "Fehler beim Serialisieren nach " << full_path << ": " << e.what() << std::endl;
        return false;
    }
    datei.close();
    return true;
}
