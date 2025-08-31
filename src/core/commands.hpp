/*
 * commands.hpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_COMMANDS_HPP_
#define SRC_CORE_COMMANDS_HPP_

#import "common.hpp"
#include "state.hpp"

enum class SectionCommandType {
    Off, On, Out, Set, Color
};

class SectionGlobalCommand {
public:
    int brightness;
    int density;
    SectionEffectType effect;

    SectionGlobalCommand(int b, int d, SectionEffectType e) :
            brightness(b), density(d), effect(e) {
    }
};

class SectionOnCommand: SectionGlobalCommand {
public:
    SectionOnCommand(int b, int d, SectionEffectType e) :
            SectionGlobalCommand(b, d, e) {
    }
};
class SectionOffCommand {
};
class SectionOutCommand {
};

class SectionColorRangeCommand {
    int index;
};

class SectionColorCommand: SectionColorRangeCommand {
    bool isFrom;
    RGB rgb;
};
class SectionInterpolationCommand: SectionColorRangeCommand {
    RgbInterpolationType interpolation;
    double midpoint; // >0 to <1 default .5 Solve to get the quadratic coeficients
    bool seamless;
    bool animating;
    int frameDuration;
    int cycleSpeed;
};
class SectionSetCommand: SectionGlobalCommand {
};

class SectionStateCommand {
    SectionCommandType type;
    Section section;
    union {
        SectionOnCommand on;
        SectionOffCommand off;
        SectionOutCommand out;
        SectionColorCommand color;
        SectionInterpolationCommand interpolation;
        SectionSetCommand set;
    } command;

};

enum class GlobalCommandType {
    Off, On
};

class GlobalOnCommand {
public:
    int brightness;

    GlobalOnCommand(int b) :
            brightness(b) {
    }
};

class GlobalOffCommand {
public:
};

struct GlobalStateCommand {
public:
    GlobalCommandType type;
    union {
        GlobalOnCommand on;
        GlobalOffCommand off;
    };
};

enum class CmdIsGlobal {
    Global, Section
};

struct Command {
public:
    CmdIsGlobal type;
    union {
        GlobalStateCommand global;
        SectionStateCommand section;
    };
};


extern void handleCommand(const GlobalOnCommand &cmd);
extern void handleCommand(const GlobalOffCommand &cmd);


#endif /* SRC_CORE_COMMANDS_HPP_ */
