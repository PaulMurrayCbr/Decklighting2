/*
 * api.cpp
 *
 * This method converts a web request into a command as described in state.hpp, sends that command to the pixel state mutator,
 * and returns the current pixel state in the response.
 *
 * Currently, our api is entirely querystring based, so even though you can post json, the json is ignored.
 *
 *
 * TODO: save and load config. Now that we have a filesystem, it would be nice to be able to save an load effect state and global state
 * with useful names. Effect state should not include which section - it should just be 'Dusk Ambience', global state would be
 * 'brightness 16, Dusk Ambience for FrontDoor, GameRoom merged, everything else off'
 *
 * Ugh - so the state for a section could either be 'loaded from a file', with a default of plain white, and an 'edited' flag.
 * This might be too hard.
 *
 * Since we need to return the altered state, the only mutex being passed to the pixel loop would be, section by section, 'unchanged'/'changed'/'requires restart',
 * same for the global.
 *
 *  Created on: 23 Aug 2025
 *      Author: pmurray
 */

#include <iostream>
#include <sstream>

#include "api.hpp"
#include "state.hpp"
#include "commands.hpp"

GlobalState state;

namespace {

    std::pair<int, json> sample(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        json j = "Not Implemented: ";
        return {404, j};

    }

    int toInt(const std::string &s, int min, int max) {
        int v = std::stoi(s);
        if (v < min || v > max) {
            throw std::out_of_range(s + " must be " + std::to_string(min) + "-" + std::to_string(max));
        }
        return v;
    }

    int toByte(const std::string &s) {
        return toInt(s, 0, 255);
    }

    std::pair<int, json> handle_global_on(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {

        int brightness = -1;

        auto p = params.find("brightness");
        if (p != params.end()) {
            brightness = toByte(p->second);
        }

        handleCommand(GlobalOnCommand(brightness));
        return {200, "should be turning on real soon"};

    }

    std::pair<int, json> handle_global_off(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        handleCommand(GlobalOffCommand());
        return {200, "should be turning off real soon"};

    }
    std::pair<int, json> handle_section_on(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        json j = "Not Implemented: ";
        return {404, j};

    }
    std::pair<int, json> handle_section_off(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        json j = "Not Implemented: ";
        return {404, j};

    }
    std::pair<int, json> handle_section_out(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        json j = "Not Implemented: ";
        return {404, j};

    }
    std::pair<int, json> handle_section_color(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        json j = "Not Implemented: ";
        return {404, j};

    }
    std::pair<int, json> handle_section_set(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        json j = "Not Implemented: ";
        return {404, j};

    }

    std::pair<int, json> handle_section(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        std::string frag;
        std::getline(path, frag, '/');

        if (frag.empty() || frag == "status") {
            return {200, "this is the section  " + sectionName(section) + " status json"};

        } else if (frag == "on") {
            return handle_section_on(section, path, params, command);
        } else if (frag == "off") {
            return handle_section_off(section, path, params, command);
        } else if (frag == "out") {
            return handle_section_out(section, path, params, command);
        } else if (frag == "color" || frag == "c" || frag == "colour") {
            return handle_section_color(section, path, params, command);
        } else if (frag == "set") {
            return handle_section_set(section, path, params, command);
        } else {
            throw std::out_of_range(frag + " is not a section command");
        }

        return {404, "handling section " + sectionName(section) + " command " + frag + " not implemented"};

    }

    std::pair<int, json> handle_api(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {

        std::string frag;
        std::getline(path, frag, '/');

        std::cout << "fragment at handle_api " << frag << "\n";

        if (!path || frag.empty() || frag == "status") {
            json j = "This is the global status json";
            return {200, j};
        } else if (frag == "on") {
            return handle_global_on(path, params, command);

        } else if (frag == "off") {
            return handle_global_off(path, params, command);

        } else {
            // if it's not a global on or off, then it must be a section

            return handle_section(sectionLookup(frag), path, params, command);
        }

    }

    std::pair<int, json> handle_root(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        std::string frag;
        std::getline(path, frag, '/');

        // ignore the initial blank token before the inital '/'

        if (!frag.empty()) {
            json j = "well, that was wierd: " + frag;
            return {500, j};
        }

        // ok, now it should be /api

        std::getline(path, frag, '/');
        std::cout << "fragment at handle_root " << frag << "\n";

        if (!path || frag != "api") {
            json j = "Not Implemented: " + frag;
            return {404, j};
        } else {
            return handle_api(path, params, command);
        }

    }

}

std::pair<int, json> api(const std::string &path, const std::multimap<std::string, std::string> &params, json &command) {
    try {
        std::cout << "path is " << path << "\n";
        std::stringstream ss(path);
        return handle_root(ss, params, command);
    } catch (const std::out_of_range &e) {
        return {400, e.what()};
    } catch (const std::exception &e) {
        return {500, e.what()};
    }
}
