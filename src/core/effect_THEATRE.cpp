/*
 * effect_THEATRE.cpp
 *
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#include "common.hpp"
#include "state.hpp"

#include "effect.hpp"

namespace {
    void run(SectionState &s, bool repaint_required) {
    }

}

void (*effectRunner_THEATRE)(SectionState &s, bool repaint_required) = run;
