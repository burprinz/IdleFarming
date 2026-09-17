#pragma once

//
// Created by luisb on 16.09.2026.
//

#pragma once
//
// Created by luisb on 19.08.2026.
//

#include <map>
#include <string>
#include <unordered_map>

class Language {

public:

    static Language& instance() {
        static Language instance;
        return instance;
    }

    inline std::unordered_map<std::string, std::string>& multiplier() {return multipliers;}
    inline std::unordered_map<std::string, std::string>& button() {return buttons;}


private:

    std::unordered_map<std::string, std::string> multipliers;
    std::unordered_map<std::string, std::string> buttons;

};
