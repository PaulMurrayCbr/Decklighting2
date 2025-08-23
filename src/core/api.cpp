/*
 * api.cpp
 *
 *  Created on: 23 Aug 2025
 *      Author: pmurray
 */

#include "api.hpp"

std::pair<int, json> api(const std::string &path, const std::multimap<std::string, std::string> &params, json& command) {
	json j = "Not Implemented: " + path;
	return {404, j};

}
