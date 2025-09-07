/*
 * effect_THEATRE.cpp
 * Two interleaved solid colours, either of which might be gradiated.
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

    void repaint_evens(SectionState &s) {
        interpolate_color(s.colors[0], 0, 1);

        int npixels = (s.npixels() + 1) / 2;

        for (int i = 0; i < npixels; i++) {
            RGB rgb = interpolate_color(s.colors[0], i, npixels);
            set_pixel(s.pixel(i * 2), rgb);
        }

    }

    void repaint_odds(SectionState &s) {
        int npixels = s.npixels() / 2;

        for (int i = 0; i < npixels; i++) {
            RGB rgb = interpolate_color(s.colors[1], i, npixels);
            set_pixel(s.pixel(i * 2 + 1), rgb);
        }

    }

    void repaint(SectionState &s) {
        repaint_evens(s);
        repaint_odds(s);
    }

    bool animate(SectionState &s) {
        bool repainted = false;
        if (s.colors[0].animation.nextFrameReady()) {
            repaint_evens(s);
            repainted = true;
        }
        if (s.colors[1].animation.nextFrameReady()) {
            repaint_odds(s);
            repainted = true;
        }

        return repainted;
    }

}

void (*effectRepaint_THEATRE)(SectionState &s) = repaint;
bool (*effectAnimate_THEATRE)(SectionState &s) = animate;
