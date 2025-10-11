#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <iostream>
#include <string>

#include "../lib/json.hpp"
using json = nlohmann::json;

#include "save.hpp"

namespace {
    const std::string path = "saves";
    const std::string ext = ".json";

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
        // todo: check filename and are you sure
        // get json from command-json
        // serialise it to file
    }

    void local_load(Section section, std::string name) {
        // todo: read file into a json object
        // tell command-json to apply it

    }

}

json getSaves() {
    return local_getSaves();
}

void save(Section section, std::string name, bool areYouSure) {
    local_save(section, name, areYouSure);
}

void load(Section section, std::string name) {
    local_load(section, name);

}
