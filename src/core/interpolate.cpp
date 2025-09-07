/*
 * interpolate.cpp
 *
 *  Created on: 7 Sept 2025
 *      Author: pmurray
 */

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

        int apparentr = APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.r] + (APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.to.r] - APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.r]) * relative;
        int apparentg = APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.g] + (APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.to.g] - APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.g]) * relative;
        int apparentb = APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.b] + (APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.to.b] - APPARENT_BRIGHTNESS_OF_PIX_VALUE[c.from.b]) * relative;

        return RGB(PIX_VALUE_FOR_APPARENT_BRIGHTNESS[apparentr], PIX_VALUE_FOR_APPARENT_BRIGHTNESS[apparentb], PIX_VALUE_FOR_APPARENT_BRIGHTNESS[apparentb]);
    }
}

RGB interpolate_color(ColorRangeState &c, int pix, int of) {
    if (c.seamless) {
// it's not obvious, but this works for odds and for evens
        of = of / 2 + 1;
        if (pix >= of) {
            pix -= of;
            pix = of - pix - 2;
        }
    }

    // lets just do this using floating point for now

    double relative = (double) pix / (double) of;

    if (c.animation.animating) {
        relative += (double) c.animation.currentCyclePos / (double) c.animation.cycleSpeed;
        while (relative > 1.0)
            relative -= 1;

    }

    switch (c.interpolation) {
#define INTERPOLATION(name) case RgbInterpolationType::name : return INTERP_##name::compute(c, relative);
    INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
}}
