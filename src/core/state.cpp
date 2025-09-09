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
#include <iterator> // for std::begin, std::end

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
        state["bias"] = s.bias;
        state["seamless"] = s.seamless;
        state["interpolation"] = interpolationName(s.interpolation);
        state["cycleSpeed"] = s.animation.cycleSpeed;
        return state;
    }

    json toJson(Section section) {
        try {
            json state = { };
            SectionState &s = sharedState.section[static_cast<size_t>(section)];

            state["mode"] = s.mode == SectionMode::on ? "on" : s.mode == SectionMode::off ? "off" : "out";
            state["density"] = s.density;

            state["effect"] = effectName(s.effect);

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

            for (int i = 0; i < NSECTIONS; i++) {
                state[sectionName(i)] = toJson(static_cast<Section>(i));
            }

            return state;
        } catch (const std::exception &e) {
            auto msg = std::string("toJson(): ") + e.what();
            std::cerr << msg;
            throw std::logic_error(msg);
        }
    }

    json globalInfo() {
        json info = { };

        info["effects"] = json::array( {
#define EFFECT(name) #name,
                EFFECT_TYPE_LIST
#undef EFFECT
                });

        info["interpolations"] = json::array( {
#define INTERPOLATION(name) #name,
                INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
                });

        info["sections"] = json::array( {
#define SECTION(name) #name,
                SECTION_LIST
#undef SECTION
                });

        info["ncolorranges"] = NCOLORANGES;

        return info;
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

json getGlobalInfo() {
    return globalInfo();
}

// must be invoked inside a mutex
void recompute_sections() {
    if (sharedState.section[0].mode == SectionMode::out) {
        sharedState.section[0].mode = SectionMode::off;
        sharedState.section[0].touched = true;
        sharedState.section[0].needsRepaint = true;
    }

    int start = 0;
    int length = 0;

    for (int i = NSECTIONS - 1; i >= 0; i--) {
        length += SECTION_LEN[i];

        SectionState &s = sharedState.section[i];

        if (s.mode == SectionMode::out) {
            continue;
        }

        if (s.length != length || sharedState.section[i].start != start) {
            s.touched = true;
            s.needsRepaint = true;
            s.length = length;
            s.start = start;
        }

        start += length;
        length = 0;
    }
}

