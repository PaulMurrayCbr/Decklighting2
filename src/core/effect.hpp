/*
 * effect.hpp
 *
 *  Created on: 1 Sept 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_EFFECT_HPP_
#define SRC_CORE_EFFECT_HPP_

#include "common.hpp"
#include "state.hpp"

extern void (*const EFFECT_REPAINT[])(SectionState&);
extern bool (*const EFFECT_ANIMATE[])(SectionState&);

#endif /* SRC_CORE_EFFECT_HPP_ */
