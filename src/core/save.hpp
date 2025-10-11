#ifndef SRC_CORE_SAVE_HPP
#define SRC_CORE_SAVE_HPP

#include "../lib/json.hpp"
using json = nlohmann::json;

#include "common.hpp"

class SaveLoadException: public std::exception {
public:
    const std::string message;
    const std::string file;

    explicit SaveLoadException(const std::string &file, const std::string &msg) :
            file(file), message(msg) {
    }

    const char* what() const noexcept override {
        return message.c_str();
    }

};

// this is a separate subclass because the front-end needs to prompt, not simply display the error
class AreYouSureException: public SaveLoadException {
public:
    explicit AreYouSureException(const std::string &file) :
            SaveLoadException(file, file + " already exists") {
    }

};

extern json getSaves();
extern void saveSection(Section section, std::string name, bool areYouSure);
extern void loadSection(Section section, std::string name);

#endif
