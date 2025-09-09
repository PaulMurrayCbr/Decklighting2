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

const std::map<std::string, SectionEffectType> EFFECT_TYPE_ENUM_OF = {
#define EFFECT(name) { #name, SectionEffectType::name},
        EFFECT_TYPE_LIST
#undef EFFECT
        };

const std::map<std::string, RgbInterpolationType> INTERPOLATION_TYPE_ENUM_OF = {
#define INTERPOLATION(name) { #name, RgbInterpolationType::name},
        INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
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

RGB hsq2rgb(HSQ hsq) {
    RGB rgb;

    hsq.h = std::max(0, std::min(APPARENT_BRIGHTNESS_SCALE * 6 - 1, (int) hsq.h));
    hsq.s = std::min(hsq.s, hsq.v);
    uint16_t lo_b = hsq.v - hsq.s;

    int segment = hsq.h / APPARENT_BRIGHTNESS_SCALE; // this should compile down to a bit operation
    int hueval = hsq.h % APPARENT_BRIGHTNESS_SCALE; // this should compile down to a bit operation

    // the hue val does not go up to the saturation, because hue brighness equal to the brightest brightness
    // means that we use the *next* color band and the hueval becomes zero
    // so we use integer division.

    hueval = (hueval * (int) hsq.s) / APPARENT_BRIGHTNESS_SCALE;

    switch (segment) {
    case HUESEGMENT::RY:
        rgb.r = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v];
        rgb.g = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[lo_b + hueval];
        rgb.b = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hsq.s];
        break;
    case HUESEGMENT::YG:
        rgb.g = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v];
        rgb.r = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hueval];
        rgb.b = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hsq.s];
        break;
    case HUESEGMENT::GC:
        rgb.g = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v];
        rgb.b = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[lo_b + hueval];
        rgb.r = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hsq.s];
        break;
    case HUESEGMENT::CB:
        rgb.b = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v];
        rgb.g = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hueval];
        rgb.r = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hsq.s];
        break;
    case HUESEGMENT::BM:
        rgb.b = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v];
        rgb.r = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[lo_b + hueval];
        rgb.g = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hsq.s];
        break;
    case HUESEGMENT::MR:
        rgb.r = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v];
        rgb.b = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hueval];
        rgb.g = PIX_VALUE_FOR_APPARENT_BRIGHTNESS[hsq.v - hsq.s];
        break;
    }

    return rgb;
}

HSQ rgb2hsq(RGB rgb) {
    HSQ hsq;

    const int16_t r = APPARENT_BRIGHTNESS_OF_PIX_VALUE[rgb.r];
    const int16_t g = APPARENT_BRIGHTNESS_OF_PIX_VALUE[rgb.g];
    const int16_t b = APPARENT_BRIGHTNESS_OF_PIX_VALUE[rgb.b];

    hsq.v = std::max(r, std::max(g, b));
    uint16_t small = std::min(r, std::min(g, b));
    hsq.s = hsq.v - small;

    if (hsq.s == 0) {
        hsq.h = 0; // the default "hue" of grey is red.
        return hsq;
    }

    int16_t mid;
    if (hsq.v == r)
        mid = std::max(g, b);
    else if (hsq.v == g)
        mid = std::max(r, b);
    else
        mid = std::max(r, g);

    // note, at this point, that mid might be equal to the min or the max, but never both.

    // is it a shade from red to yellow?
    // if the biggest value is red, and the smallest value is blue, and green is not also the biggest value
    // is it a shade from yellow to green?
    // if the biggest value is green, and the smallest value is blue, and red is not also the smallest value

    HUESEGMENT seg;

    if (hsq.v == r && small == b && hsq.v != g) {
        seg = HUESEGMENT::RY;
    } else if (hsq.v == g && small == b && small != r) {
        seg = HUESEGMENT::YG;
    } else if (hsq.v == g && small == r && hsq.v != b) {
        seg = HUESEGMENT::GC;
    } else if (hsq.v == b && small == r && small != g) {
        seg = HUESEGMENT::CB;
    } else if (hsq.v == b && small == g && hsq.v != r) {
        seg = HUESEGMENT::BM;
    } else if (hsq.v == r && small == g && small != b) {
        seg = HUESEGMENT::MR;
    } else {
        // this should never happen
        throw std::logic_error("can't figure out colours correctly");
    }

    hsq.h = seg * APPARENT_BRIGHTNESS_SCALE;

    if (seg % 2 == 0) {
        // going from a primary color to a composite color
        // value goes from dimmest to not including the brightest

        hsq.h += (((int) mid - (int) small) * APPARENT_BRIGHTNESS_SCALE) / hsq.s;
    } else {
        // going from a composite color to a primary color
        // value goes from brightest to not including the dimmest

        hsq.h += APPARENT_BRIGHTNESS_SCALE - 1 - (((int) mid - (int) small) * APPARENT_BRIGHTNESS_SCALE) / hsq.s;
    }

    return hsq;
}

// 'brightness' ranges 0 up to (but not including) 480, because this gives us enough
// distinct values to map both directions.

const int16_t APPARENT_BRIGHTNESS_OF_PIX_VALUE[] = { 0, 29, 42, 51, 59, 67, 73, 79, 84, 89, 94, 99, 103, 108, 112, 116,  //
        119, 123, 127, 130, 134, 137, 140, 143, 146, 149, 152, 155, 158, 161, 164, 167,  //
        169, 172, 174, 177, 179, 182, 184, 187, 189, 192, 194, 196, 198, 201, 203, 205,  //
        207, 209, 212, 214, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 238,  //
        239, 241, 243, 245, 247, 249, 250, 252, 254, 256, 258, 259, 261, 263, 264, 266,  //
        268, 269, 271, 273, 274, 276, 278, 279, 281, 282, 284, 286, 287, 289, 290, 292,  //
        293, 295, 296, 298, 299, 301, 302, 304, 305, 307, 308, 310, 311, 313, 314, 316,  //
        317, 318, 320, 321, 323, 324, 325, 327, 328, 329, 331, 332, 334, 335, 336, 338,  //
        339, 340, 342, 343, 344, 345, 347, 348, 349, 351, 352, 353, 354, 356, 357, 358,  //
        359, 361, 362, 363, 364, 366, 367, 368, 369, 371, 372, 373, 374, 375, 377, 378,  //
        379, 380, 381, 382, 384, 385, 386, 387, 388, 389, 391, 392, 393, 394, 395, 396,  //
        397, 399, 400, 401, 402, 403, 404, 405, 406, 407, 409, 410, 411, 412, 413, 414,  //
        415, 416, 417, 418, 419, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431,  //
        432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447,  //
        448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463,  //
        464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479,  //
        };

const uint8_t PIX_VALUE_FOR_APPARENT_BRIGHTNESS[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,  //
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,  //
        2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4,  //
        4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7,  //
        7, 7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10,  //
        10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13,  //
        14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18,  //
        18, 18, 19, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23,  //
        23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28,  //
        28, 29, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34,  //
        34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 40,  //
        41, 41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 47, 47, 48,  //
        48, 49, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55,  //
        56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 64,  //
        64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 70, 70, 71, 71, 72, 72,  //
        73, 73, 74, 75, 75, 76, 76, 77, 78, 78, 79, 79, 80, 81, 81, 82,  //
        82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 89, 89, 90, 90, 91, 92,  //
        92, 93, 94, 94, 95, 96, 96, 97, 98, 98, 99, 100, 100, 101, 102, 102,  //
        103, 104, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110, 111, 112, 113, 113,  //
        114, 115, 115, 116, 117, 118, 118, 119, 120, 121, 121, 122, 123, 123, 124, 125,  //
        126, 126, 127, 128, 129, 129, 130, 131, 132, 133, 133, 134, 135, 136, 136, 137,  //
        138, 139, 140, 140, 141, 142, 143, 144, 144, 145, 146, 147, 148, 148, 149, 150,  //
        151, 152, 152, 153, 154, 155, 156, 157, 157, 158, 159, 160, 161, 162, 163, 163,  //
        164, 165, 166, 167, 168, 169, 169, 170, 171, 172, 173, 174, 175, 176, 176, 177,  //
        178, 179, 180, 181, 182, 183, 184, 185, 185, 186, 187, 188, 189, 190, 191, 192,  //
        193, 194, 195, 196, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,  //
        208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,  //
        224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,  //
        240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,  //
        };
