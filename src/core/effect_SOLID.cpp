/*
 * effect_SOLID.cpp
 * A single color that can be animated
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#include <iostream>

#include "common.hpp"
#include "state.hpp"

#include "effect.hpp"

namespace {
    void repaint(SectionState &s) {
        std::cout << "repainting SOLID from " << s.start << " length " << s.length;
    }

    bool animate(SectionState &s) {
        return false;
    }
}

void (*effectRepaint_SOLID)(SectionState &s) = repaint;
bool (*effectAnimate_SOLID)(SectionState &s) = animate;
