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

#include "effect.hpp"

namespace {
    void repaint(SectionState &s) {
        std::cout << "repainting GRADIENT from " << s.start << " length " << s.length;
    }

    bool animate(SectionState &s) {
        return false;
    }
}

void (*effectRepaint_GRADIENT)(SectionState &s) = repaint;
bool (*effectAnimate_GRADIENT)(SectionState &s) = animate;
