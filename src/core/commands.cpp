/*
 * commands.cpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#include <stdexcept>
#include <iostream>
#include <chrono>

#include "common.hpp"
#include "state.hpp"
#include "commands.hpp"

namespace {

    void doSectionGlobal(const SectionGlobalCommand &cmd) {
        SectionState &s = sharedState.section[static_cast<int>(cmd.section)];

        if (cmd.density.has_value()) {
            if (*cmd.density <= 0) {
                throw new std::logic_error("density must be > 0");
            }
            s.density = *cmd.density;
            s.touched = true;
            s.needsRepaint = true;
        }
        if (cmd.effect.has_value()) {
            s.effect = *cmd.effect;
            s.touched = true;
            s.needsRepaint = true;
        }
    }
}

void handleCommand(const GlobalOnCommand &cmd) {
    inSharedStateMutex([cmd] {
        sharedState.on = true;
        if (cmd.brightness.has_value()) {
            sharedState.brightness = *cmd.brightness;
        }
        sharedState.touched = true;
        sharedState.needsRepaint = true;
    });
}

void handleCommand(const GlobalOffCommand &cmd) {
    inSharedStateMutex([cmd] {
        sharedState.on = false;
        sharedState.touched = true;
        sharedState.needsRepaint = true;
    });
}

void handleCommand(const SectionOnCommand &cmd) {
    inSharedStateMutex([cmd] {
        doSectionGlobal(cmd);
        SectionState &s = sharedState.section[static_cast<int>(cmd.section)];
        s.mode = SectionMode::on;
        s.touched = true;
        s.needsRepaint = true;
        recompute_sections();
    });
}

void handleCommand(const SectionOutCommand &cmd) {
    inSharedStateMutex([cmd] {
        SectionState &s = sharedState.section[static_cast<int>(cmd.section)];
        s.mode = SectionMode::out;
        s.touched = true;
        s.needsRepaint = true;
        recompute_sections();
    });
}

void handleCommand(const SectionOffCommand &cmd) {
    inSharedStateMutex([cmd] {
        SectionState &s = sharedState.section[static_cast<int>(cmd.section)];
        s.mode = SectionMode::off;
        s.touched = true;
        s.needsRepaint = true;
        recompute_sections();
    });
}

void handleCommand(const SectionSetCommand &cmd) {
    inSharedStateMutex([cmd] {
        doSectionGlobal(cmd);
    });
}

void handleCommand(const SectionColorCommand &cmd) {
    inSharedStateMutex([cmd] {
        SectionState &s = sharedState.section[static_cast<int>(cmd.section)];
        ColorRangeState &r = s.colors[cmd.index];
        if (cmd.rgb.has_value()) {
            if (cmd.isFrom) {
                r.from = *cmd.rgb;
                r.fromHsv = rgb2hsv(r.from);
            } else {
                r.to = *cmd.rgb;
                r.toHsv = rgb2hsv(r.to);
            }

            s.touched = true;
            s.needsRepaint = true;
        }
    });
}

void handleCommand(const SectionInterpolationCommand &cmd) {
    inSharedStateMutex([cmd] {
        SectionState &s = sharedState.section[static_cast<int>(cmd.section)];
        ColorRangeState &r = s.colors[cmd.index];

        if (cmd.interpolation.has_value()) {
            r.interpolation = *cmd.interpolation;
            s.touched = true;
            s.needsRepaint = true;
        }
        if (cmd.midpoint.has_value()) {
            r.midpoint = *cmd.midpoint;
            s.touched = true;
            s.needsRepaint = true;
        }
        if (cmd.seamless.has_value()) {
            r.seamless = *cmd.seamless;
            s.touched = true;
            s.needsRepaint = true;
        }
        if (cmd.animating.has_value()) {
            r.animation.animating = *cmd.animating;
            s.touched = true;
        }
        if (cmd.frameDuration.has_value()) {
            r.animation.frameDuration = std::chrono::milliseconds(*cmd.frameDuration);
            s.touched = true;
        }
        if (cmd.cycleSpeed.has_value()) {
            r.cycleSpeed = std::chrono::milliseconds(*cmd.cycleSpeed);
            s.touched = true;
        }
    });
}
