/*
 * interpolate.cpp
 *
 *  Created on: 7 Sept 2025
 *      Author: pmurray
 */

#include <cmath>

#include "common.hpp"
#include "state.hpp"

#include "interpolate.hpp"

namespace INTERP_NONE {
    RGB compute(ColorRangeState &c, double relative) {
        return c.from;
    }

}

namespace INTERP_FADE {
    // this won't be correct because the 'to' end gets clipped
    RGB compute(ColorRangeState &c, double relative) {
        return RGB( //
                (int) ((double) c.from.r + (double) ((int) c.to.r - (int) c.from.r) * relative), //
                (int) ((double) c.from.g + (double) ((int) c.to.g - (int) c.from.g) * relative), //
                (int) ((double) c.from.b + (double) ((int) c.to.b - (int) c.from.b) * relative));
    }

}

namespace INTERP_QFADE {
    // this won't be correct because the 'to' end gets clipped
    RGB compute(ColorRangeState &c, double relative) {
        // this won't be correct because the 'to' end gets clipped

        int apparentr = APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.r] + ((int) APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.to.r] - (int) APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.r]) * relative;
        int apparentg = APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.g] + ((int) APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.to.g] - (int) APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.g]) * relative;
        int apparentb = APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.b] + ((int) APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.to.b] - (int) APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.b]) * relative;

        apparentr = apparentr >= 0 ? apparentr < APPARENT_BRIGHTNESS_SCALE ? apparentr : APPARENT_BRIGHTNESS_SCALE : 0;
        apparentg = apparentg >= 0 ? apparentg < APPARENT_BRIGHTNESS_SCALE ? apparentg : APPARENT_BRIGHTNESS_SCALE : 0;
        apparentb = apparentb >= 0 ? apparentb < APPARENT_BRIGHTNESS_SCALE ? apparentb : APPARENT_BRIGHTNESS_SCALE : 0;

        return RGB(PIX_VALUE_FOR_APPARENT_BRIGHTNESS[apparentr], PIX_VALUE_FOR_APPARENT_BRIGHTNESS[apparentg], PIX_VALUE_FOR_APPARENT_BRIGHTNESS[apparentb]);
    }
}

inline double bias(double x, double a) {
    return x / ((1.0 / a - 2.0) * (1.0 - x) + 1.0);
}

RGB interpolate_color(ColorRangeState &c, int pix, int of) {
    // lets just do this using floating point for now

    double relative = (double) pix / (double) of;

    if (c.animation.animating) {
        relative += (double) c.animation.currentCyclePos / (double) c.animation.cycleSpeed;
        while (relative > 1.0)
            relative -= 1;

    }

    relative = relative / ((1 / c.bias - 2) * (1 - relative) + 1);


    if (c.seamless) {
        relative *= 2;
        if (relative >= 1) {
            relative = 1 - (relative - 1);
        }
    }

    relative = std::fmod(relative, 1);

    switch (c.interpolation) {
#define INTERPOLATION(name) case RgbInterpolationType::name : return INTERP_##name::compute(c, relative);
    INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
}}
