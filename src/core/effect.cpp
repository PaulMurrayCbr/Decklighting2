/*
 * effect.cpp
 *
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#include <unordered_map>
#include <functional>

#include "common.hpp"
#include "state.hpp"

#include "effect.hpp"

extern void (*effectRunner_SOLID)(SectionState&, bool);
extern void (*effectRunner_THEATRE)(SectionState&, bool);

const std::unordered_map<SectionEffectType, void (*)(SectionState&, bool), EnumClassHash> EFFECT_RUNNERS = //
        { //
        { SectionEffectType::SOLID, effectRunner_SOLID }, //
                { SectionEffectType::THEATRE, effectRunner_THEATRE } //
        };
