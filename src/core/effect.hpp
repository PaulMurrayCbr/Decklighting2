/*
 * effect.hpp
 *
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_EFFECT_HPP_
#define SRC_CORE_EFFECT_HPP_

#include <unordered_map>

#include "common.hpp"
#include "state.hpp"

extern const std::unordered_map<SectionEffectType, void (*)(SectionState&, bool), EnumClassHash> EFFECT_RUNNERS;

#endif /* SRC_CORE_EFFECT_HPP_ */
