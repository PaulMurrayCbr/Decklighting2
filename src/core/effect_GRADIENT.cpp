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
    auto anim = false;
    void repaint(SectionState &s) {
        if(!anim)
        std::cout << " doing a gradient repaint for " << s.start << " len " << s.length << "\n";

        int npixels = s.npixels();

        if(!anim)
        std::cout << " calculating " << npixels << " pixels\n";

        for (int i = 0; i < npixels; i++) {
            RGB rgb = interpolate_color(s.colors[0], i, npixels);
            set_pixel(s.pixel(i), rgb);
        }
    }

    bool animate(SectionState &s) {
        if (s.colors[0].animation.nextFrameReady()) {
            anim = true;
            repaint(s);
            anim = false;
            return true;
        } else {
            return false;
        }
    }
}

void (*effectRepaint_GRADIENT)(SectionState &s) = repaint;
bool (*effectAnimate_GRADIENT)(SectionState &s) = animate;
