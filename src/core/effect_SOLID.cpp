/*
 * effect_SOLID.cpp
 * A single color that can be animated
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#include "common.hpp"
#include "state.hpp"
#include "pixels.hpp"
#include "interpolate.hpp"

#include "effect.hpp"

namespace {
    void repaint(SectionState &s) {
        int npixels = s.npixels();

        RGB rgb = interpolate_color(s.colors[0], 0, 1);

        for (int i = 0; i < npixels; i++) {
            set_pixel(s.pixel(i), rgb);
        }
    }

    bool animate(SectionState &s) {
        if (s.colors[0].animation.nextFrameReady()) {
            repaint(s);
            return true;
        } else {
            return false;
        }
    }
}

void (*effectRepaint_SOLID)(SectionState &s) = repaint;
bool (*effectAnimate_SOLID)(SectionState &s) = animate;
