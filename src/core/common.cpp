/*
 * common.cpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#include <algorithm>  // for std::max
#include <stdexcept>  // for std::max

#include "common.hpp"

const std::string EFFECT_TYPE_NAME[] = {
#define EFFECT(name) #name,
        EFFECT_TYPE_LIST
#undef EFFECT
        };

const std::string INTERPOLATION_TYPE_NAME[] = {
#define INTERPOLATION(name) #name,
        INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
        };

const std::string SECTION_NAME[] = {
#define SECTION(name) #name,
        SECTION_LIST
#undef SECTION
        };

const std::unordered_map<SectionEffectType, std::string, EnumClassHash> EFFECT_TYPE_NAME_OF = {
#define EFFECT(name) { SectionEffectType::name, #name},
        EFFECT_TYPE_LIST
#undef EFFECT
        };

const std::map<std::string, SectionEffectType> EFFECT_TYPE_ENUM_OF = {
#define EFFECT(name) { #name, SectionEffectType::name},
        EFFECT_TYPE_LIST
#undef EFFECT
        };

const std::unordered_map<RgbInterpolationType, std::string, EnumClassHash> INTERPOLATION_TYPE_NAME_OF = {
#define INTERPOLATION(name) { RgbInterpolationType::name, #name},
        INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
        };

const std::map<std::string, RgbInterpolationType> INTERPOLATION_TYPE_ENUM_OF = {
#define INTERPOLATION(name) { #name, RgbInterpolationType::name},
        INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
        };

const std::unordered_map<Section, std::string, EnumClassHash> SECTION_NAME_OF = {
#define SECTION(name) { Section::name, #name},
        SECTION_LIST
#undef SECTION
        };

const std::map<std::string, Section> SECTION_ENUM_OF = {
#define SECTION(name) { #name, Section::name},
        SECTION_LIST
#undef SECTION
        };

const int SECTION_LEN[NSECTIONS] = {
#define SECTION(name) name##_LEN ,
        SECTION_LIST
#undef SECTION

        };

// maybe move this stuff to rgbhsv.hpp so it can be inlined

/**
 * Hue is a value from 0 to 256*6-1.
 *   0- 255 : red->yellow   (r, g, b)
 *  256- 511: yellow->green (g, r, b)
 *  512- 767: green->cyan (g, b, r)
 *  768-1023: cyan->blue (b, g, r)
 * 1224-1479: blue->magenta (b, r, g)
 * 1480-1735: magenta->red (r, b, g)
 *
 * Brightness is the value of the biggest color
 * Sat is the difference between highest and lowest.
 */

enum HUESEGMENT {
    RY, YG, GC, CB, BM, MR
};

RGB hsv2rgb(HSV hsb) {
    RGB rgb;

    hsb.h = std::max(0, std::min(256 * 6 - 1, hsb.h));
    hsb.s = std::min(hsb.s, hsb.v);
    uint8_t lo_b = hsb.v - hsb.s;

    int segment = hsb.h / 256; // this should compile down to a bit operation
    int hueval = hsb.h % 256; // this should compile down to a bit operation

    // the hue val does not go up to the saturation, because hue brighness equal to the brightest brightness
    // means that we use the *next* color band and the hueval becomes zero
    // so we use integer division.

    hueval = (hueval * (int) hsb.s) / 256;

    switch (segment) {
    case HUESEGMENT::RY:
        rgb.r = hsb.v;
        rgb.g = lo_b + hueval;
        rgb.b = hsb.v - hsb.s;
        break;
    case HUESEGMENT::YG:
        rgb.g = hsb.v;
        rgb.r = hsb.v - hueval;
        rgb.b = hsb.v - hsb.s;
        break;
    case HUESEGMENT::GC:
        rgb.g = hsb.v;
        rgb.b = lo_b + hueval;
        rgb.r = hsb.v - hsb.s;
        break;
    case HUESEGMENT::CB:
        rgb.b = hsb.v;
        rgb.g = hsb.v - hueval;
        rgb.r = hsb.v - hsb.s;
        break;
    case HUESEGMENT::BM:
        rgb.b = hsb.v;
        rgb.r = lo_b + hueval;
        rgb.g = hsb.v - hsb.s;
        break;
    case HUESEGMENT::MR:
        rgb.r = hsb.v;
        rgb.b = hsb.v - hueval;
        rgb.g = hsb.v - hsb.s;
        break;
    }

    return rgb;
}

HSV rgb2hsv(RGB rgb) {
    HSV hsb;

    hsb.v = std::max(rgb.r, std::max(rgb.g, rgb.b));
    uint8_t small = std::min(rgb.r, std::min(rgb.g, rgb.b));
    hsb.s = hsb.v - small;

    if (hsb.s == 0) {
        hsb.h = 0; // the default "hue" of grey is red.
        return hsb;
    }

    uint8_t mid;
    if (hsb.v == rgb.r)
        mid = std::max(rgb.g, rgb.b);
    else if (hsb.v == rgb.g)
        mid = std::max(rgb.r, rgb.b);
    else
        mid = std::max(rgb.r, rgb.g);

    // note, at this point, that mid might be equal to the min or the max, but never both.

    // is it a shade from red to yellow?
    // if the biggest value is red, and the smallest value is blue, and green is not also the biggest value
    // is it a shade from yellow to green?
    // if the biggest value is green, and the smallest value is blue, and red is not also the smallest value

    HUESEGMENT seg;

    if (hsb.v == rgb.r && small == rgb.b && hsb.v != rgb.g) {
        seg = HUESEGMENT::RY;
    } else if (hsb.v == rgb.g && small == rgb.b && small != rgb.r) {
        seg = HUESEGMENT::YG;
    } else if (hsb.v == rgb.g && small == rgb.r && hsb.v != rgb.b) {
        seg = HUESEGMENT::GC;
    } else if (hsb.v == rgb.b && small == rgb.r && small != rgb.g) {
        seg = HUESEGMENT::CB;
    } else if (hsb.v == rgb.b && small == rgb.g && hsb.v != rgb.r) {
        seg = HUESEGMENT::BM;
    } else if (hsb.v == rgb.r && small == rgb.g && small != rgb.b) {
        seg = HUESEGMENT::MR;
    } else {
        // this should never happen
        throw std::logic_error("can't figure out colours correctly");
    }

    hsb.h = seg * 256;

    if (seg % 2 == 0) {
        // going from a primary color to a composite color
        // value goes from dimmest to not including the brightest

        hsb.h += (((int) mid - (int) small) * 256) / hsb.s;
    } else {
        // going from a composite color to a primary color
        // value goes from brightest to not including the dimmest

        hsb.h += 255 - (((int) mid - (int) small) * 256) / hsb.s;
    }

    return hsb;
}

