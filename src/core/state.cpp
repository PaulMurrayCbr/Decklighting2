/*
 * state.cpp
 *
 *  Created on: 24 Aug 2025
 *      Author: pmurray
 */

#include "state.hpp"

const std::string EFFECT_TYPE_NAME[] = {
#define EFFECT(name) #name,
		EFFECT_TYPE_LIST
#undef EFFECT
		};

const std::string INTERPOLATION_TYPE_NAME[] = {
#define INTERPOLATION(name) #name,
		INTERPOLATION_TYPE_LIST
#undef INTERPOLATION
		};

const std::string SECTION_NAME[] = {
#define SECTION(name) #name,
		SECTION_LIST
#undef SECTION

		};

GlobalState sharedState;
std::mutex sharedStateMutex;
