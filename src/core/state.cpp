/*
 * state.cpp
 *
 *  Created on: 24 Aug 2025
 *      Author: pmurray
 */

#include <stdexcept>
#include <iostream>
#include <sstream>   // for std::stringstream
#include <string>    // for std::string
#include <iomanip>   // for std::setw, std::setfill

#include "common.hpp"
#include "state.hpp"

GlobalState sharedState;
std::mutex sharedStateMutex;

namespace {

    json toJson(RGB &rgb) {
        std::stringstream ss;
        ss << "#" << std::hex << std::uppercase //
                << std::setw(2) << std::setfill('0') << (rgb.r & 0xFF) //
                << std::setw(2) << std::setfill('0') << (rgb.g & 0xFF) //
                << std::setw(2) << std::setfill('0') << (rgb.b & 0xFF);
        return ss.str();
    }

    json toJson(ColorRangeState &s) {
        json state = { };
        //AnimationState animation;
        state["animating"] = s.animation.animating;
        state["frameDuration"] = s.animation.frameDuration.count();
        state["from"] = toJson(s.from);
        state["to"] = toJson(s.to);
        state["midpoint"] = s.midpoint;
        state["seamless"] = s.seamless;
        state["cycleSpeed"] = s.cycleSpeed.count();
        return state;
    }

    json toJson(Section section) {
        try {
            json state = { };
            SectionState &s = sharedState.section[static_cast<size_t>(section)];

            state["mode"] = s.mode == SectionMode::on ? "on" : s.mode == SectionMode::off ? "off" : "out";
            state["density"] = s.density;

            state["effect"] = EFFECT_TYPE_NAME_OF.at(s.effect);

            state["color"] = json::array();

            for (int i = 0; i < NCOLORANGES; i++) {
                state["color"] += toJson(s.colors[i]);
            }

            return state;
        } catch (const std::exception &e) {
            std::ostringstream oss;
            oss << "toJson( " << static_cast<int>(section) << "): " << e.what();
            auto msg = oss.str();
            std::cerr << msg;
            throw std::logic_error(msg);
        }
    }

    json toJson() {
        try {
            json state = { };

            state["on"] = sharedState.on;
            state["brightness"] = sharedState.brightness;

            for (const auto &kv : SECTION_NAME_OF) {
                state[kv.second] = toJson(kv.first);
            }

            return state;
        } catch (const std::exception &e) {
            auto msg = std::string("toJson(): ") + e.what();
            std::cerr << msg;
            throw std::logic_error(msg);
        }
    }

}

json getGlobalState() {
    return inSharedStateMutex([]() {
        return toJson();
    });

}

json getSectionState(Section section) {
    return inSharedStateMutex([section]() {
        return toJson(section);
    });
}

// must be invoked inside a mutex
void recompute_sections() {
    std::cout << "Need to recompute sections" << '\n';
    std::cout << "There are " << NSECTIONS << " sections" << '\n';
    std::cout << "There are " << NPIXELS << " pixels" << '\n';

    if (sharedState.section[0].mode == SectionMode::out) {
        sharedState.section[0].mode = SectionMode::off;
        sharedState.section[0].touched = true;
        sharedState.section[0].needsRepaint = true;
    }

    int start = 0;
    int length = 0;

    for (int i = NSECTIONS - 1; i >= 0; i--) {
        length += SECTION_LEN[i];

        if (sharedState.section[i].mode == SectionMode::out) {
            std::cout << "Section " << i << " is out. Adding " << SECTION_LEN[i] << " to the next section" << '\n';
            continue;
        }

        std::cout << "Section " << i << " will start at " << start << " and have a length of " << length << '\n';

        if (sharedState.section[i].length != length || sharedState.section[i].start != start) {
            sharedState.section[0].touched = true;
            sharedState.section[0].needsRepaint = true;

            sharedState.section[i].length = length;
            sharedState.section[i].start = start;
        }

        start += length;
        length = 0;
    }
}

