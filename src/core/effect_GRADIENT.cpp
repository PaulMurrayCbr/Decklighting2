/*
 * effect_GRADIENT.cpp
 * A color gradient that can be animated
 *
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#include <iostream>

#include "common.hpp"
#include "state.hpp"
#include "pixels.hpp"
#include "interpolate.hpp"

#include "effect.hpp"

namespace {
    void repaint(SectionState &s) {
        std::cout << "repainting GRADIENT from " << s.start << " length " << s.length;
        int npixels = s.npixels();

        for (int i = 0; i < npixels; i++) {
            RGB rgb = getColorAt(s.colors[0]); // , i, npixels);
            set_pixel(s.pixel(i), rgb);
        }

        foo();
    }

    bool animate(SectionState &s) {
        return false;
    }
}

void (*effectRepaint_GRADIENT)(SectionState &s) = repaint;
bool (*effectAnimate_GRADIENT)(SectionState &s) = animate;
