/*
 * interpolate.cpp
 *
 *  Created on: 7 Sept 2025
 *      Author: pmurray
 */

#include <cmath>
#include <iostream>
#include <string>

#include "common.hpp"
#include "state.hpp"

#include "interpolate.hpp"

namespace {
    // this method always does roygbv interpolation
    inline HSQ interpolate_hsq(HSQ a, HSQ b, double relative) {
        int16_t ah = a.h;
        int16_t bh = b.h;

        while (ah < 0)
            ah += APPARENT_BRIGHTNESS_SCALE * 6;
        while (bh < 0)
            bh += APPARENT_BRIGHTNESS_SCALE * 6;
        while (bh < ah) {
            bh += APPARENT_BRIGHTNESS_SCALE * 6;
        }

        int16_t h = ah + (bh - ah) * relative;

        while (h < 0)
            h += APPARENT_BRIGHTNESS_SCALE * 6;
        while (h >= APPARENT_BRIGHTNESS_SCALE * 6)
            h -= APPARENT_BRIGHTNESS_SCALE * 6;
        int16_t v = a.v + (b.v - a.v) * relative;
        int16_t s = a.s + (b.s - a.s) * relative;
        v = v < 0 ? 0 : v >= APPARENT_BRIGHTNESS_SCALE ? APPARENT_BRIGHTNESS_SCALE - 1 : v;
        s = s < 0 ? 0 : s > v ? v : s;
        return HSQ(h, s, v);
    }
}

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

namespace INTERP_ROYGBV {
    RGB compute(ColorRangeState &c, double relative) {
        return hsq2rgb(interpolate_hsq(c.fromHsq, c.toHsq, relative));
    }
}

namespace INTERP_VBGYOR {
    RGB compute(ColorRangeState &c, double relative) {
        return hsq2rgb(interpolate_hsq(c.toHsq, c.fromHsq, relative));
    }
}

namespace INTERP_NEAREST {
    // no, this is tricky. I'll get some sleep try tomorrow

    RGB compute(ColorRangeState &c, double relative) {
        int16_t dist = c.toHsq.v - c.fromHsq.v;
        if (dist < 0)
            dist += APPARENT_BRIGHTNESS_SCALE * 6;
        if (c.toHsq.v - c.fromHsq.v < APPARENT_BRIGHTNESS_SCALE * 3)
            return hsq2rgb(interpolate_hsq(c.fromHsq, c.toHsq, relative));
        else
            return hsq2rgb(interpolate_hsq(c.toHsq, c.fromHsq, relative));
    }
}

namespace INTERP_FURTHEST {
    RGB compute(ColorRangeState &c, double relative) {
        if (c.toHsq.v - c.fromHsq.v < APPARENT_BRIGHTNESS_SCALE * 3)
            return hsq2rgb(interpolate_hsq(c.fromHsq, c.toHsq, relative));
        else
            return hsq2rgb(interpolate_hsq(c.toHsq, c.fromHsq, relative));
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

HSQ TEST_INTERP(HSQ a, HSQ b, double relative) {
    return interpolate_hsq(a, b, relative);
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
