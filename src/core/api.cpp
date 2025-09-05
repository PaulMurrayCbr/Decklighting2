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
#include <stdexcept>
#include <limits>

#include "commands.hpp"
#include "state.hpp"

#include "api.hpp"

namespace {
    int toInt(const std::string &s, int min, int max) {
        int v = std::stoi(s);
        if (v < min || v > max) {
            throw std::out_of_range(s + " must be " + std::to_string(min) + "-" + std::to_string(max));
        }
        return v;
    }

    int toDouble(const std::string &s, double min, double max) {
        double v = std::stod(s);
        if (v < min || v > max) {
            throw std::out_of_range(s + " must be " + std::to_string(min) + "-" + std::to_string(max));
        }
        return v;
    }

    bool toBool(const std::string &s) {
        if (s == "y" || s == "Y" || s == "on" || s == "true" || s == "yes")
            return true;
        if (s == "n" || s == "N" || s == "off" || s == "false" || s == "no")
            return false;

        throw std::out_of_range(s + " doesn't look like a yes/no to me");
    }

    int toByte(const std::string &s) {
        return toInt(s, 0, 255);
    }

    inline void ifHasByteParam(const std::multimap<std::string, std::string> &params, const std::string &key, const std::function<void(int)> &f) {
        auto it = params.find(key);
        if (it != params.end()) {
            f(toByte(it->second));
        }
    }

    inline void ifHasStringParam(const std::multimap<std::string, std::string> &params, const std::string &key, const std::function<void(std::string)> &f) {
        auto it = params.find(key);
        if (it != params.end()) {
            f(it->second);
        }
    }

    std::pair<int, json> handle_global_on(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        GlobalOnCommand cmd;
        ifHasByteParam(params, "brightness", [&cmd](int v) {
            cmd.brightness = v;
        });
        handleCommand(cmd);
        return {200, getGlobalState()};

    }

    std::pair<int, json> handle_global_off(std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        handleCommand(GlobalOffCommand());
        return {200, getGlobalState()};

    }

    void populateSectionEffectCommand(EffectSOLIDCommand &cmd, const std::multimap<std::string, std::string> &params, json &command) {

    }

    void populateSectionEffectCommand(EffectGRADIENTCommand &cmd, const std::multimap<std::string, std::string> &params, json &command) {

    }

    void populateSectionEffectCommand(EffectTHEATRECommand &cmd, const std::multimap<std::string, std::string> &params, json &command) {

    }

    void populateSectionGlobalCommand(SectionGlobalCommand &cmd, const std::multimap<std::string, std::string> &params, json &command) {
        ifHasByteParam(params, "density", [&cmd](int v) {
            cmd.density = v;
        });

        ifHasStringParam(params, "effect", [&cmd, &params, &command](std::string v) {
            cmd.effect = EFFECT_TYPE_ENUM_OF.at(v);

            switch (*cmd.effect) {
            case SectionEffectType::SOLID:
                populateSectionEffectCommand(cmd.eff.solid, params, command);
                break;

            case SectionEffectType::GRADIENT:
                populateSectionEffectCommand(cmd.eff.gradient, params, command);
                break;

            case SectionEffectType::THEATRE:
                populateSectionEffectCommand(cmd.eff.theatre, params, command);
                break;
            }

        });
    }

    std::pair<int, json> handle_section_on(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        SectionOnCommand cmd = SectionOnCommand(section);
        populateSectionGlobalCommand(cmd, params, command);
        handleCommand(cmd);
        return {200, getSectionState(section)};

    }
    std::pair<int, json> handle_section_off(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        SectionOffCommand cmd = SectionOffCommand(section);
        handleCommand(cmd);
        return {200, getSectionState(section)};
    }
    std::pair<int, json> handle_section_out(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        SectionOutCommand cmd = SectionOutCommand(section);
        handleCommand(cmd);
        return {200, getSectionState(section)};

    }
    std::pair<int, json> handle_section_set(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        SectionSetCommand cmd = SectionSetCommand(section);
        populateSectionGlobalCommand(cmd, params, command);
        handleCommand(cmd);
        return {200, getSectionState(section)};

    }

    std::pair<int, json> handle_section_color_interpolation(Section section, int range, const std::multimap<std::string, std::string> &params, json &command) {
        SectionInterpolationCommand cmd = SectionInterpolationCommand(section, range);

        ifHasStringParam(params, "interpolation", [&cmd](std::string v) {
            cmd.interpolation = INTERPOLATION_TYPE_ENUM_OF.at(v);
        });

        ifHasStringParam(params, "midpoint", [&cmd](std::string v) {
            cmd.midpoint = toDouble(v, 0, 1);
        });

        ifHasStringParam(params, "seamless", [&cmd](std::string v) {
            cmd.seamless = toBool(v);
        });

        ifHasStringParam(params, "animating", [&cmd](std::string v) {
            cmd.animating = toBool(v);
        });

        ifHasStringParam(params, "frameDuration", [&cmd](std::string v) {
            cmd.frameDuration = toInt(v, 0, std::numeric_limits<int>::max());
        });
        ifHasStringParam(params, "cycleSpeed", [&cmd](std::string v) {
            cmd.cycleSpeed = toInt(v, 0, std::numeric_limits<int>::max());
        });

        handleCommand(cmd);

        return {200, getSectionState(section)};
    }

    std::pair<int, json> handle_section_color_rgb(Section section, int rangeNo, std::string fromTo, const std::multimap<std::string, std::string> &params, json &command) {

        bool isFrom;

        if (fromTo == "from") {
            isFrom = true;
        } else if (fromTo == "to") {
            isFrom = false;
        } else {
            throw std::logic_error("expected 'from' or 'to'");
        }

        SectionColorCommand cmd = SectionColorCommand(section, rangeNo, isFrom);

        int r = -1;
        int g = -1;
        int b = -1;

        ifHasByteParam(params, "r", [&r](int v) {
            r = v;
        });
        ifHasByteParam(params, "g", [&g](int v) {
            g = v;
        });
        ifHasByteParam(params, "b", [&b](int v) {
            b = v;
        });

        if ((r == -1 || g == -1 || b == -1) && (r != -1 || g != -1 || b != -1)) {
            throw std::logic_error("expected all of r, g, and b");
        }

        cmd.rgb = RGB(r, g, b);

        handleCommand(cmd);

        return {200, getSectionState(section)};
    }

    std::pair<int, json> handle_section_color(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        std::string range;
        std::getline(path, range, '/');

        if (range.empty()) {
            throw std::logic_error("expected section range");
        }

        int rangeNo = toInt(range, 0, NCOLORANGES);

        std::string fromTo;
        std::getline(path, fromTo, '/');

        if (fromTo.empty()) {
            return handle_section_color_interpolation(section, rangeNo, params, command);
        } else {
            return handle_section_color_rgb(section, rangeNo, fromTo, params, command);
        }
    }

    std::pair<int, json> handle_section(Section section, std::stringstream &path, const std::multimap<std::string, std::string> &params, json &command) {
        std::string frag;
        std::getline(path, frag, '/');

        if (frag.empty() || frag == "status") {
            return {200, getSectionState(section)};

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

        if (!path || frag.empty() || frag == "status") {
            return {200, getGlobalState()};
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
        std::stringstream ss(path);
        return handle_root(ss, params, command);
    } catch (const std::out_of_range &e) {
        return {400, e.what()};
    } catch (const std::exception &e) {
        return {500, e.what()};
    }
}
