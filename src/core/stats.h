#pragma once
//
// Created by luisb on 19.08.2026.
//

#include <map>
#include <string>

class StatsManager {

public:

    static StatsManager& instance() {
        static StatsManager instance;
        return instance;
    }

    inline void init() noexcept {
        stats["manual_planted"] = 0;
        stats["auto_planted"] = 0;
        stats["manual_harvested"] = 0;
        stats["auto_harvested"] = 0;
        stats["tools_bought"] = 0; // TODO
        stats["items_sold"] = 0; // TODO
    }

    inline void set(std::string name, int value) {
        stats[name] = value;
    }

    inline int get(std::string name) {
        auto i = stats.find(name);
        return i != stats.end() ? i->second : 0;
    }

    inline void higher(std::string name, int value=1) {
        stats[name] += value;
    }

    inline std::map<std::string, int> get_map() {
        return stats;
    }


private:

    std::map<std::string, int> stats;

};
