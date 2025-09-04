/*
 * effect_SOLID.cpp
 *
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#include "common.hpp"
#include "state.hpp"

#include "effect.hpp"

namespace {
    void repaint(SectionState &s) {
    }

    bool animate(SectionState &s) {
        return false;
    }
}

void (*effectRepaint_SOLID)(SectionState &s) = repaint;
bool (*effectAnimate_SOLID)(SectionState &s) = animate;
