/**
 * Data structures used for communicating state from the http server to the pixels.
 *
 */

#ifndef SRC_CORE_STATE_HPP
#define SRC_CORE_STATE_HPP

#include <cstdint>
#include <chrono>


/*
 * so:
 * #define STR(x) #x
 * #define SECTION(S) \
 *   if(foo == Section::S) { \
 *     cout << "the name of the section is " << STR(s) << "\n"; \
 *   }
 * SECTION_LIST
 * #undef SECTION
 *
 * I think
 */

#define EFFECT_TYPE_LIST \
    EFFECT(SOLID) \
    EFFECT(THEATRE)

#define INTERPOLATION_TYPE_LIST \
	INTERPOLATION(FADE)

#define SECTION_LIST \
	SECTION(Door) \
	SECTION(Game) \
	SECTION(Theatre) \
	SECTION(Deck) \
	SECTION(Back)

static const int NSECTIONS = 5;
static const int NCOLORANGES = 2;

enum class SectionEffectType {
#define EFFECT(name) name,
	EFFECT_TYPE_LIST
#undef EFFECT
};


enum class RgbInterpolationType {
#define INTERPOLATION(name) name,
	INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
};

enum class Section {
#define SECTION(name) name,
	SECTION_LIST
#undef SECTION
};

enum class SectionMode { on, off, out };

// these state values need to be structures that can be assigned with =

struct RGB {
	uint8_t r { 0 }, g { 0 }, b { 0 };
};


struct AnimationState {
	bool animating;
	int animationFrame;
	std::chrono::milliseconds frameDuration;
	std::chrono::time_point<std::chrono::steady_clock> previousFrameTime;

	void setFrameDuration(double ms) {
		frameDuration = std::chrono::milliseconds(static_cast<int64_t>(ms));
	}

	void restart() {
		previousFrameTime = std::chrono::steady_clock::now() - frameDuration;
	}

	/** note that this method sets the current frame time if it returns true
	 * We do this immediately so that the frame rate is not affected by the time i takes to render the animation
	 */

	bool nextFrameReady() {
		auto now = std::chrono::steady_clock::now();

		if(now - previousFrameTime >= frameDuration) {
			previousFrameTime = now;
			return true;
		}
		else {
			return false;
		}
	}
};

// dont need this here
//class Animator {
//	virtual void animate() = 0;
//};
//
//#define EFFECT(name) class Animate_##name : Animator;
//    EFFECT_LIST
//#undef EFFECT

struct EffectSOLIDState {

};

struct EffectTHEATREState {

};

struct ColorRangeState {
	AnimationState animation;
	RGB from;
	RGB to;
};


struct SectionState {
	SectionMode mode;
	int brightness;
	int density;
	bool needsReset;

	SectionEffectType effect;
	AnimationState animation;

	ColorRangeState colors[NCOLORANGES];

	union {
#define EFFECT(name) Effect##name##State name;
		EFFECT_TYPE_LIST
#undef EFFECT
	} effectState;
};

struct GlobalState {
	bool on;
	int brightness;
	bool needsReset;

	struct SectionState section[NSECTIONS];

};

#endif
