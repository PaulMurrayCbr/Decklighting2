/*
 * commands.hpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_COMMANDS_HPP_
#define SRC_CORE_COMMANDS_HPP_

#include <functional>

#include "../lib/optional.hpp"
#include "common.hpp"
#include "state.hpp"

class SectionCommand {
public:
    Section section;

    SectionCommand(Section section) :
            section(section) {

    }
};

class EffectSOLIDCommand {
public:
    void load(const EffectSOLIDState &of);
};

class EffectGRADIENTCommand {
public:
    void load(const EffectGRADIENTState &of);
};

class EffectTHEATRECommand {
public:
    void load(const EffectTHEATREState &of);
};

class SectionGlobalCommand: public SectionCommand {
public:
    Optional<int> density;
    Optional<SectionEffectType> effect;

    // todo: this perhaps should be a list of optionals rather than being a union
    // because unions and C++ copy constructors don't play nice.

    union {
        EffectSOLIDCommand solid;
        EffectGRADIENTCommand gradient;
        EffectTHEATRECommand theatre;
    } eff;

    SectionGlobalCommand(Section section) :
            SectionCommand(section) {

    }

    void load(const SectionState &of);
};

class SectionOnCommand: public SectionGlobalCommand {
public:
    SectionOnCommand(Section section) :
            SectionGlobalCommand(section) {
    }
    std::function<void()> buildExecutor() const;
};

class SectionOffCommand: public SectionCommand {
public:
    SectionOffCommand(Section section) :
            SectionCommand(section) {
    }

    std::function<void()> buildExecutor() const;
};

class SectionOutCommand: public SectionCommand {
public:
    SectionOutCommand(Section section) :
            SectionCommand(section) {

    }
    std::function<void()> buildExecutor() const;
};

class SectionColorRangeCommand: public SectionCommand {
public:
    int index;

    SectionColorRangeCommand(Section section, int index) :
            SectionCommand(section), index(index) {

    }
};

class SectionColorCommand: public SectionColorRangeCommand {
public:
    bool isFrom;
    Optional<RGB> rgb;

    SectionColorCommand(Section section, int index, bool isFrom) :
            SectionColorRangeCommand(section, index), isFrom(isFrom) {

    }

    void load(const RGB &of);
    std::function<void()> buildExecutor() const;
};

class SectionInterpolationCommand: public SectionColorRangeCommand {
public:
    Optional<RgbInterpolationType> interpolation;
    Optional<double> bias; // >0 to <1 default .5
    Optional<bool> seamless;
    Optional<bool> animating;
    Optional<int> frameDuration;
    Optional<int> cycleSpeed;

    SectionInterpolationCommand(Section section, int index) :
            SectionColorRangeCommand(section, index) {

    }

    void load(const ColorRangeState &of);
    std::function<void()> buildExecutor() const;
};

class SectionSetCommand: public SectionGlobalCommand {
public:
    SectionSetCommand(Section section) :
            SectionGlobalCommand(section) {

    }
    std::function<void()> buildExecutor() const;
};

class GlobalOnCommand {
public:
    Optional<int> brightness;
};

class GlobalOffCommand {
public:
};

extern void handleCommand(const GlobalOnCommand &cmd);
extern void handleCommand(const GlobalOffCommand &cmd);
extern void handleCommand(const SectionOnCommand &cmd);
extern void handleCommand(const SectionOutCommand &cmd);
extern void handleCommand(const SectionOffCommand &cmd);
extern void handleCommand(const SectionSetCommand &cmd);
extern void handleCommand(const SectionColorCommand &cmd);
extern void handleCommand(const SectionInterpolationCommand &cmd);

#endif /* SRC_CORE_COMMANDS_HPP_ */
