/*
 * common.hpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_COMMON_HPP_
#define SRC_CORE_COMMON_HPP_

#include <string>
#include <cstdint>
#include <map>

#define EFFECT_TYPE_LIST \
    EFFECT(SOLID) \
    EFFECT(GRADIENT) \
    EFFECT(THEATRE)

#define INTERPOLATION_TYPE_LIST \
    INTERPOLATION(NONE) \
    INTERPOLATION(FADE) \
    INTERPOLATION(QFADE) \
    INTERPOLATION(ROYGBV) \
    INTERPOLATION(VBGYOB)

#define SECTION_LIST \
    SECTION(Door) \
    SECTION(Game) \
    SECTION(Theatre) \
    SECTION(Deck) \
    SECTION(Back)

static const int NSECTIONS = 0
#define SECTION(name) +1
        SECTION_LIST
#undef SECTION
;

//const int Back_LEN = 3;
//const int Deck_LEN = 3;
//const int Theatre_LEN = 4;
//const int Game_LEN = 3;
//const int Door_LEN = 3;

const int Back_LEN = 253;
const int Deck_LEN = 192;
const int Theatre_LEN = 205;
const int Game_LEN = 249;
const int Door_LEN = 247;

const int NPIXELS = 0
#define SECTION(name) + name##_LEN
        SECTION_LIST
#undef SECTION
;

extern const int SECTION_LEN[NSECTIONS];

const int NCOLORANGES = 2;

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

extern const std::string EFFECT_TYPE_NAME[];
extern const std::string INTERPOLATION_TYPE_NAME[];
extern const std::string SECTION_NAME[];

extern const std::map<std::string, SectionEffectType> EFFECT_TYPE_ENUM_OF;
extern const std::map<std::string, RgbInterpolationType> INTERPOLATION_TYPE_ENUM_OF;
extern const std::map<std::string, Section> SECTION_ENUM_OF;

inline std::string sectionName(int section) {
    return SECTION_NAME[section];
}
inline std::string sectionName(Section section) {
    return SECTION_NAME[static_cast<int>(section)];
}
inline std::string effectName(int effect) {
    return EFFECT_TYPE_NAME[effect];
}
inline std::string effectName(SectionEffectType effect) {
    return EFFECT_TYPE_NAME[static_cast<int>(effect)];
}
inline std::string interpolationName(int interpolation) {
    return INTERPOLATION_TYPE_NAME[interpolation];
}
inline std::string interpolationName(RgbInterpolationType interpolation) {
    return INTERPOLATION_TYPE_NAME[static_cast<int>(interpolation)];
}

inline Section sectionLookup(std::string name) {
    return SECTION_ENUM_OF.at(name);
}
inline SectionEffectType effectLookup(std::string name) {
    return EFFECT_TYPE_ENUM_OF.at(name);
}
inline RgbInterpolationType interpolationLookup(std::string name) {
    return INTERPOLATION_TYPE_ENUM_OF.at(name);
}

enum class SectionMode {
    on, off, out
};

// these state values need to be structures that can be assigned with =

struct RGB {
    uint8_t r { 0 }, g { 0 }, b { 0 };

    RGB() {
    }

    RGB(uint8_t r, uint8_t g, uint8_t b) :
            r(r), g(g), b(b) {
    }

    RGB(int r, int g, int b) :
            r(r), g(g), b(b) {
    }
};

struct HSV {
    // hue goes from 0 to 256 (minus 1) * 6 .
    int h { 0 };
    uint8_t s { 0 }, v { 0 };

    HSV() {
    }

    HSV(int h, int s, int v) :
            h(h), s(s), v(v) {
    }

    HSV(int h, uint8_t s, uint8_t v) :
            h(h), s(s), v(v) {
    }
};

extern RGB hsv2rgb(HSV c);
extern HSV rgb2hsv(RGB c);

// 'brightness' ranges 0 up to (but not including) 480, because this gives us enough
// distinct values to map from rgb and then back again.
// x == PIX_VALUE_FOR_APPARENT_BRIGHTNESS[APPARENT_BRIGHTNESS_OF_PIX_VALUE[x]];
// The converse is not true - there are
// not enough pixel levels to distinguish all possible brighnesses from 0-479, but really, that's fine

// do be sure to clamp your computed brightness values - it's an array access.

const int APPARENT_BRIGHTNESS_SCALE = 480;
extern const int16_t APPARENT_BRIGHTNESS_OF_PIX_VALUE[];
extern const uint8_t PIX_VALUE_FOR_APPARENT_BRIGHTNESS[];

struct HSQ {
    // hue goes from 0 to APPARENT_BRIGHTNESS_SCALE * 6 (minus 1).
    int16_t h { 0 };
    // brghtness and saturation likewise go from 0 APPARENT_BRIGHTNESS_SCALE  (minus 1).
    int16_t s { 0 }, v { 0 };

    HSQ() {
    }

    HSQ(int h, int s, int v) :
            h(h), s(s), v(v) {
    }

    HSQ(int16_t h, int16_t s, int16_t v) :
            h(h), s(s), v(v) {
    }
};

extern RGB hsq2rgb(HSQ c);
extern HSQ rgb2hsq(RGB c);

#endif /* SRC_CORE_COMMON_HPP_ */
