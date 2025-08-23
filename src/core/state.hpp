/**
 * Data structures used for communicating state from the http server to the pixels.
 *
 */

#ifndef SRC_CORE_STATE_HPP
#define SRC_CORE_STATE_HPP

#include <cstdint>

enum class SectionEffectType {
	SOLID, THEATRE
};

enum class RgbInterpolationType {
	FADE
};

struct RGB {
	uint8_t r { 0 }, g { 0 }, b { 0 };
};

enum class SectionCommandType {
	Off, On, Brightness, Density, Color, Speed, Effect
};

enum class Section {
	Door, Game, Theatre, Deck, Back
};

class SectionOnCommand {
};
class SectionOffCommand {
};
class SectionBrightnessCommand {
	int brightness;
};
class SectionDensityCommand {
	int density;
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
};
class SectionSpeedCommand {
	int speed;
};
class SectionEffectCommand {
	SectionEffectType effect;
};

class SectionStateCommand {
	SectionCommandType type;
	Section section;
	union {
		SectionOnCommand on;
		SectionOffCommand off;
		SectionBrightnessCommand brighness;
		SectionDensityCommand density;
		SectionColorCommand color;
		SectionInterpolationCommand interpolation;
		SectionSpeedCommand speed;
		SectionEffectCommand effect;
	};

};

enum class GlobalCommandType {
	Off, On, Brightness
};

class GlobalOnCommand {
};
class GlobalOffCommand {
};
class GlobalBrightnessCommand {
	int brightness;
};

struct GlobalStateCommand {
	GlobalCommandType type;
};

enum class CmdIsGlobal {
	Global, Effect
};

struct Command {
	CmdIsGlobal type;
	union {
		GlobalStateCommand global;
		SectionStateCommand section;
	};
};

#endif
