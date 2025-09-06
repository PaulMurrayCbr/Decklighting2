/*
 * interpolate.cpp
 *
 *  Created on: 7 Sept 2025
 *      Author: pmurray
 */

#include "common.hpp"
#include "state.hpp"

#include "interpolate.hpp"

RGB getColorAt(ColorRangeState c, int pix, int of) {
    if (c.seamless) {
        // it's not obvious, but this works for odds and for evens
        of = of / 2 + 1;
        if (pix >= of) {
            pix -= of;
            pix = of - pix - 2;
        }
    }

    // this will be different for animating and non-animating, as we want the animation
    // to be smooth, so floating point will be easiest.

    // to do it in integer, we would want the lowest common multiple of the cycle speed ms and the number of pixels
    // might be able to get away with rounding things to 1 in 2^15



    return c.from;
}
