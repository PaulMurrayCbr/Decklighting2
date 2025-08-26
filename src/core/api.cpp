/*
 * api.cpp
 *
 * This method converts a web request into a command as described in state.hpp, sends that command to the pixel state mutator,
 * and returns the current pixel state in the response.
 *
 * Currently, our api is entirely querystring based, so even though you can post json, the json is ignored.
 *
 *
 * TODO: save and load config. Now that we have a filesystem, it would be nice to be able to save an load effect state and global state
 * with useful names. Effect state should not include which section - it should just be 'Dusk Ambience', global state would be
 * 'brightness 16, Dusk Ambience for FrontDoor, GameRoom merged, everything else off'
 *
 * Ugh - so the state for a section could either be 'loaded from a file', with a default of plain white, and an 'edited' flag.
 * This might be too hard.
 *
 * Since we need to return the altered state, the only mutex being passed to the pixel loop would be, section by section, 'unchanged'/'changed'/'requires restart',
 * same for the global.
 *
 *  Created on: 23 Aug 2025
 *      Author: pmurray
 */

#include "api.hpp"
#include "state.hpp"

GlobalState state;

std::pair<int, json> api(const std::string &path,
		const std::multimap<std::string, std::string> &params, json &command) {
	json j = "Not Implemented: " + path;
	return {404, j};

}
