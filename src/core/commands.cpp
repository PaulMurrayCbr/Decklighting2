/*
 * commands.cpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#include <stdexcept>

#include "common.hpp"
#include "state.hpp"
#include "commands.hpp"

void handleCommand(const GlobalOnCommand &cmd) {
    inSharedStateMutex([&] {
        sharedState.on = true;
        if (cmd.brightness != -1) {
            sharedState.brightness = cmd.brightness;
        }
        sharedState.touched = true;
        sharedState.needsRepaint = true;
    });
}

void handleCommand(const GlobalOffCommand &cmd) {
    inSharedStateMutex([&] {
        sharedState.on = false;
        sharedState.touched = true;
        sharedState.needsRepaint = true;
    });
}

void handleCommand(const SectionOnCommand &cmd) {
    throw std::runtime_error("SectionOnCommand not implemented yet");
}

void handleCommand(const SectionOutCommand &cmd) {
    throw std::runtime_error("SectionOutCommand not implemented yet");
}

void handleCommand(const SectionOffCommand &cmd) {
    throw std::runtime_error("SectionOffCommand not implemented yet");
}

void handleCommand(const SectionSetCommand &cmd) {
    throw std::runtime_error("SectionSetCommand not implemented yet");
}

void handleCommand(const SectionColorCommand &cmd) {
    throw std::runtime_error("SectionColorCommand not implemented yet");
}

void handleCommand(const SectionInterpolationCommand &cmd) {
    throw std::runtime_error("SectionInterpolationCommand not implemented yet");
}
