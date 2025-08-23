/*
 * api.hpp
 *
 *  Created on: 23 Aug 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_API_HPP_
#define SRC_CORE_API_HPP_

#include "json.hpp"

using json = nlohmann::json;

std::pair<int, json> api(const std::string &path, const std::multimap<std::string, std::string>& params, json& command);

#endif /* SRC_CORE_API_HPP_ */
