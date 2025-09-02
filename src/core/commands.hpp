/*
 * commands.hpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_COMMANDS_HPP_
#define SRC_CORE_COMMANDS_HPP_

#include "common.hpp"

class SectionCommand {
public:
    Section section;

    SectionCommand(Section section) :
            section(section) {

    }
};

class SectionGlobalCommand: public SectionCommand {
public:
    int brightness = -1;
    int density = -1;
    bool effectTouched = false;
    SectionEffectType effect = SectionEffectType::SOLID;

    SectionGlobalCommand(Section section) :
            SectionCommand(section) {

    }
};

class SectionOnCommand: public SectionGlobalCommand {
public:
    SectionOnCommand(Section section) :
            SectionGlobalCommand(section) {
    }
};

class SectionOffCommand: public SectionCommand {
public:
    SectionOffCommand(Section section) :
            SectionCommand(section) {

    }
};

class SectionOutCommand: public SectionCommand {
public:
    SectionOutCommand(Section section) :
            SectionCommand(section) {

    }
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
    RGB rgb;
    int index;

    SectionColorCommand(Section section, int index, bool isFrom) :
            SectionColorRangeCommand(section, index), isFrom(isFrom) {

    }
};

class SectionInterpolationCommand: public SectionColorRangeCommand {
public:
    RgbInterpolationType interpolation;
    double midpoint; // >0 to <1 default .5 Solve to get the quadratic coeficients
    bool seamless;
    bool animating;
    int frameDuration;
    int cycleSpeed;
    SectionInterpolationCommand(Section section, int index) :
            SectionColorRangeCommand(section, index) {

    }
};

class SectionSetCommand: public SectionGlobalCommand {
public:
    SectionSetCommand(Section section) :
            SectionGlobalCommand(section) {

    }
};

class GlobalOnCommand {
public:
    int brightness = -1;
};

class GlobalOffCommand {
public:
};

extern void handleCommand(const GlobalOnCommand &cmd);
extern void handleCommand(const GlobalOffCommand &cmd);
extern void handleCommand(const GlobalOffCommand &cmd);
extern void handleCommand(const SectionOnCommand &cmd);
extern void handleCommand(const SectionOutCommand &cmd);
extern void handleCommand(const SectionOffCommand &cmd);
extern void handleCommand(const SectionSetCommand &cmd);
extern void handleCommand(const SectionColorCommand &cmd);
extern void handleCommand(const SectionInterpolationCommand &cmd);

#endif /* SRC_CORE_COMMANDS_HPP_ */
