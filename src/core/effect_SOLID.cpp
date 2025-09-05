/*
 * effect_SOLID.cpp
 * A single color that can be animated
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#include "common.hpp"
#include "state.hpp"
#include "pixels.hpp"

#include "effect.hpp"

namespace {
    void repaint(SectionState &s) {
        int npixels = s.npixels();

        RGB rgb = s.colors[0].from;

        for (int i = 0; i < npixels; i++) {
            set_pixel(s.pixel(i), rgb.r, rgb.g, rgb.b);
        }
    }

    bool animate(SectionState &s) {
        return false;
    }
}

void (*effectRepaint_SOLID)(SectionState &s) = repaint;
bool (*effectAnimate_SOLID)(SectionState &s) = animate;
