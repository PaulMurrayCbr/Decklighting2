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
    void repaint(SectionState &s) {
        std::cout << "repainting THEATRE from " << s.start << " length " << s.length;
    }

    bool animate(SectionState &s) {
        return false;
    }
}

void (*effectRepaint_THEATRE)(SectionState &s) = repaint;
bool (*effectAnimate_THEATRE)(SectionState &s) = animate;
