#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <functional>

#include "../lib/json.hpp"
using json = nlohmann::json;

#include "common.hpp"
#include "state.hpp"
#include "commands.hpp"
#include "save.hpp"

namespace {
    const std::string path = "saves";
    const std::string ext = ".json";

    bool saveExists(const std::string &filename) {
        std::string fullPath = path + "/" + filename + ext;
        struct stat buffer;
        return (stat(fullPath.c_str(), &buffer) == 0);
    }

    json local_getSaves() {

        // thank you, chatgpt.

        nlohmann::json result = nlohmann::json::array();

        DIR *dir = opendir(path.c_str());
        if (!dir) {
            throw std::runtime_error("Cannot open directory: " + path);
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name(entry->d_name);

            if (!endsWith(name, ext))
                continue;

            std::string fullPath = path + "/" + name;
            std::string shortName = name.substr(0, name.size() - ext.size());

            // Get modification time
            struct stat fileStat;
            if (stat(fullPath.c_str(), &fileStat) != 0)
                continue; // skip if error

            std::time_t modTime = fileStat.st_mtime;
            std::tm *tmInfo = std::localtime(&modTime);

            char dateStr[11]; // YYYY-MM-DD + null terminator
            std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", tmInfo);

            // Add to JSON array as an object
            result.push_back( { { "name", shortName }, { "date", dateStr } });
        }

        closedir(dir);
        return result;
    }

    void local_save(Section section, std::string name, bool areYouSure) {
        if (saveExists(name) && !areYouSure) {
            throw AreYouSureException(name);
        }

        std::string fullPath = path + "/" + name + ext;

        json state = getSectionState(section);

        std::ofstream file(fullPath);

        if (!file.is_open()) {
            throw SaveLoadException(name, "Cannot write savefile " + fullPath);
        }

        file << state.dump(4); // pretty-print with 4-space indentation

        if (!file.good()) {
            throw SaveLoadException(name, "Cannot write savefile " + fullPath);
        }

    }

    void local_load(Section section, std::string name) {
        if (!saveExists(name)) {
            throw SaveLoadException(name, "Savefile " + name + " not found");
        }

        std::string fullPath = path + "/" + name + ext;

        std::ifstream file(fullPath);
        if (!file.is_open()) {
            // should never happen, but if it does we will supply a lower-level error
            throw SaveLoadException(name, "Cannot read savefile " + fullPath);

        }

        nlohmann::json state;
        try {
            file >> state;
        } catch (const nlohmann::json::parse_error &e) {
            throw SaveLoadException(name, "Malformed JSON in savefile " + fullPath);
        }

        std::vector < std::function < void() >> executors;

        // std::lock_guard<std::mutex> lock(sharedStateMutex);

        if (state["mode"] == "on") {
            // the on command doesn't need a 'set' command
            executors.push_back(SectionOnCommand(section).load(state).buildExecutor());
        } else if (state["mode"] == "off") {
            executors.push_back(SectionOffCommand(section).buildExecutor());
            executors.push_back(SectionSetCommand(section).load(state).buildExecutor());
        } else if (state["mode"] == "out") {
            executors.push_back(SectionOutCommand(section).buildExecutor());
            executors.push_back(SectionSetCommand(section).load(state).buildExecutor());
        }

        json colors = state["color"];
        for (size_t i = 0; i < colors.size(); ++i) {
            executors.push_back(SectionInterpolationCommand(section, i).load(colors[i]).buildExecutor());
            executors.push_back(SectionColorCommand(section, i, true).load(colors[i]["from"]).buildExecutor());
            executors.push_back(SectionColorCommand(section, i, false).load(colors[i]["to"]).buildExecutor());
        }

        inSharedStateMutex([&executors] {
            for (auto &exec : executors) {
                exec();
            }
        });

    }
}

json getSaves() {
    return local_getSaves();
}

void saveSection(Section section, std::string name, bool areYouSure) {
    local_save(section, name, areYouSure);
}

void loadSection(Section section, std::string name) {
    local_load(section, name);

}
