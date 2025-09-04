/*
 * effect.cpp
 *
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#include "common.hpp"
#include "state.hpp"

#include "effect.hpp"

#define EFFECT(name) \
    extern void (*effectRepaint_##name)(SectionState&); \
    extern bool (*effectAnimate_##name)(SectionState&);

EFFECT_TYPE_LIST
#undef EFFECT

void (*const EFFECT_REPAINT[])(SectionState&) = {
#define EFFECT(name) effectRepaint_##name,
        EFFECT_TYPE_LIST
#undef EFFECT
    };

bool (*const EFFECT_ANIMATE[])(SectionState&) = {
#define EFFECT(name) effectAnimate_##name,
        EFFECT_TYPE_LIST
#undef EFFECT
    };

