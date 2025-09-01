/*
 * state.cpp
 *
 *  Created on: 24 Aug 2025
 *      Author: pmurray
 */

#include <sstream>   // for std::stringstream
#include <string>    // for std::string
#include <iomanip>   // for std::setw, std::setfill

#import "common.hpp"
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
        json state = { };
        SectionState &s = sharedState.section[static_cast<size_t>(section)];

        state["mode"] = s.mode == SectionMode::on ? "on" : s.mode == SectionMode::off ? "off" : "out";
        state["brightness"] = s.brightness;
        state["density"] = s.density;

        state["effect"] = EFFECT_TYPE_NAME_OF.at(s.effect);

        state["color"] = json::array();

        for (int i = 0; i < NCOLORANGES; i++) {
            state["color"] += toJson(s.colors[i]);
        }

        return state;
    }

    json toJson() {
        json state = { };

        state["on"] = sharedState.on;
        state["brightness"] = sharedState.brightness;

        for (const auto &kv : SECTION_NAME_OF) {
            state[kv.second] = toJson(kv.first);
        }

        return state;
    }

}

json getGlobalState() {
    std::lock_guard<std::mutex> lock(sharedStateMutex);
    return toJson();

}

json getSectionState(Section section) {
    std::lock_guard<std::mutex> lock(sharedStateMutex);
    return toJson(section);
}

