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

#include "effect.hpp"

namespace {

    void repaint_evens(SectionState &s) {

    }

    void repaint_odds(SectionState &s) {

    }

    void repaint(SectionState &s) {
        std::cout << "repainting THEATRE from " << s.start << " length " << s.length;
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
