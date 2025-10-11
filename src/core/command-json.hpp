/*
 * command-json.hpp
 *
 *  Created on: 10 Oct2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_COMMAND_JSON_HPP_
#define SRC_CORE_COMMAND_JSON_HPP_

#include "common.hpp"

#include "../lib/json.hpp"
using json = nlohmann::json;

extern json getSectionJson(Section section);

extern void applySectionJson(Section section, json load);

#endif

