/*
 * commands.cpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#include <stdexcept>
#include <iostream>
#include <chrono>
#include <functional>

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

std::function<void()> SectionOnCommand::buildExecutor() const {
    return [this] {
        doSectionGlobal(*this);
        SectionState &s = sharedState.section[static_cast<int>(section)];
        s.mode = SectionMode::on;
        s.touched = true;
        s.needsRepaint = true;
        recompute_sections();
    };
}

void handleCommand(const SectionOnCommand &cmd) {
    inSharedStateMutex(cmd.buildExecutor());
}

std::function<void()> SectionOutCommand::buildExecutor() const {
    return [this] {
        SectionState &s = sharedState.section[static_cast<int>(section)];
        s.mode = SectionMode::out;
        s.touched = true;
        s.needsRepaint = true;

        // we also need a repaint of the section before this one now that we are linked

        for (int i = static_cast<int>(section) - 1; i >= 0; i++) {
            if (sharedState.section[i].mode != SectionMode::out) {
                sharedState.section[i].touched = true;
                sharedState.section[i].needsRepaint = true;
                break;
            }
        }

        recompute_sections();

    };
}

void handleCommand(const SectionOutCommand &cmd) {
    inSharedStateMutex(cmd.buildExecutor());
}

std::function<void()> SectionOffCommand::buildExecutor() const {
    return [this] {
        SectionState &s = sharedState.section[static_cast<int>(section)];
        s.mode = SectionMode::off;
        s.touched = true;
        s.needsRepaint = true;
        recompute_sections();
    };
}

void handleCommand(const SectionOffCommand &cmd) {
    inSharedStateMutex(cmd.buildExecutor());
}

std::function<void()> SectionSetCommand::buildExecutor() const {
    return [this] {
        doSectionGlobal(*this);
    };
}

void handleCommand(const SectionSetCommand &cmd) {
    inSharedStateMutex(cmd.buildExecutor());
}

std::function<void()> SectionColorCommand::buildExecutor() const {
    return [this] {
        SectionState &s = sharedState.section[static_cast<int>(section)];
        ColorRangeState &r = s.colors[index];
        if (rgb.has_value()) {
            if (isFrom) {
                r.from = *rgb;
                r.fromHsq = rgb2hsq(r.from);
            } else {
                r.to = *rgb;
                r.toHsq = rgb2hsq(r.to);
            }

            s.touched = true;
            s.needsRepaint = true;
        }
    };
}

void handleCommand(const SectionColorCommand &cmd) {
    inSharedStateMutex(cmd.buildExecutor());
}

std::function<void()> SectionInterpolationCommand::buildExecutor() const {
    return [this] {
        SectionState &s = sharedState.section[static_cast<int>(section)];
        ColorRangeState &r = s.colors[index];

        if (interpolation.has_value()) {
            r.interpolation = *interpolation;
            s.touched = true;
            s.needsRepaint = true;
        }
        if (bias.has_value()) {
            r.bias = *bias;
            s.touched = true;
            s.needsRepaint = true;
        }
        if (seamless.has_value()) {
            r.seamless = *seamless;
            s.touched = true;
            s.needsRepaint = true;
        }
        if (animating.has_value()) {
            r.animation.setAnimating(*animating);
            s.touched = true;
        }
        if (frameDuration.has_value()) {
            r.animation.setFrameDuration(*frameDuration);
            s.touched = true;
        }
        if (cycleSpeed.has_value()) {
            r.animation.setCycleSpeed(*cycleSpeed);
            s.touched = true;
        }
    };
}

void handleCommand(const SectionInterpolationCommand &cmd) {
    inSharedStateMutex(cmd.buildExecutor());
}

EffectSOLIDCommand& EffectSOLIDCommand::load(const json &of) {
    return *this;
}

EffectGRADIENTCommand& EffectGRADIENTCommand::load(const json &of) {
    return *this;
}

EffectTHEATRECommand& EffectTHEATRECommand::load(const json &of) {
    return *this;
}

template<typename T>
T& SectionGlobalCommandWithLoader<T>::load(const json &of) {
//    density = of.density;
//    effect = of.effect;
//
//    switch (of.effect) {
//    case SectionEffectType::SOLID:
//        eff.solid.load(of.effectState.SOLID);
//        break;
//    case SectionEffectType::GRADIENT:
//        eff.gradient.load(of.effectState.GRADIENT);
//        break;
//    case SectionEffectType::THEATRE:
//        eff.theatre.load(of.effectState.THEATRE);
//        break;
//    }
    return static_cast<T&>(*this);
}

template SectionOnCommand& SectionGlobalCommandWithLoader<SectionOnCommand>::load(const json &);
template SectionSetCommand& SectionGlobalCommandWithLoader<SectionSetCommand>::load(const json &);

SectionColorCommand& SectionColorCommand::load(const json &of) {
//    rgb = of;
    return *this;
}

SectionInterpolationCommand& SectionInterpolationCommand::load(const json &of) {
//    interpolation = of.interpolation;
//    bias = of.bias;
//    seamless = of.seamless;
//    animating = of.animation.animating;
//    frameDuration = of.animation.frameDuration.count();
//    cycleSpeed = of.animation.cycleSpeed;
    return *this;
}

