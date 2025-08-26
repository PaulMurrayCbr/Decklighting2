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

GlobalState state;

namespace {

    extern void handleCommand(GlobalOnCommand cmd) {
        inSharedStateMutex([&] {
            sharedState.on = true;
            if (cmd.brightness != -1) {
                sharedState.brightness = cmd.brightness;
            }
            sharedState.touched = true;
            sharedState.needsReset = true;
        });
    }

    extern void handleCommand(GlobalOffCommand cmd) {
        inSharedStateMutex([&] {
            sharedState.on = false;
            sharedState.touched = true;
            sharedState.needsReset = true;
        });
    }

    std::pair<int, json> sample(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        json j = "Not Implemented: ";
        return {404, j};

    }

    std::pair<int, json> handle_global_on(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {

        int brightness = -1;

        auto p = params.find("brightness");
        if (p != params.end()) {
            try {
                brightness = std::stoi(p->second);       // convert string to int

                if (brightness < 0 || brightness > 255) {
                    return {400, "bad brightness value: " + p->second};
                }
            } catch (const std::invalid_argument &e) {
                return {400, "bad brightness value: " + p->second};
            } catch (const std::out_of_range &e) {
                return {400, "bad brightness value: " + p->second};
            }
        }

        handleCommand(GlobalOnCommand(brightness));
        return {200, "should be turning on real soon"};

    }

    std::pair<int, json> handle_global_off(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        handleCommand(GlobalOffCommand());
        return {200, "should be turning off real soon"};

    }

    std::pair<int, json> handle_api(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {

        std::string frag;
        std::getline(path, frag, '/');

        std::cout << "fragment at handle_api " << frag << "\n";

        if (!path || frag.empty() || frag == "status") {
            json j = "This is the status json";
            return {200, j};
        } else if (frag == "on") {
            return handle_global_on(path, params, command);

        } else if (frag == "off") {
            return handle_global_off(path, params, command);

        } else {
            json j = "Not Implemented: " + frag;
            return {404, j};
        }

        json j = "Not Implemented: ";
        return {404, j};

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
    std::cout << "path is " << path << "\n";
    std::stringstream ss(path);
    return handle_root(ss, params, command);
}
