/*
 * api.hpp
 *
 * Since this is 'api.hpp', I suppose that here is where we document the api.
 *
 * / or /status
 *   the global status json.
 *     { on: boolean, brightness: 0-255,
 *       sections: [ list of the names of the sections],
 *       interpolations: [ list of the names of the color interpolation types ]
 *       active: [ list of the names of sections not in state merge/out]
 *     }
 * /on
 *     ?brightness=0-255 (default -1 = no change)
 *   turn stuff on
 * /off
 *   turn stuff off
 *
 * /<section> or /<section>/status status json for this section.
 *   Includes array of color ranges and for each whether from and to colours are available to be set
 * /<section>/on
 *   ?brightness=0-255 (default -1 = no change)
 *   ?density= int (default -1 = no change)
 * /<section>/off
 * /<section>/out
 * /<section>/color/<index currently 1 or 2>/<from or to>
 *   ?r 0-255
 *   ?g 0-255
 *   ?b 0-255
 * /<section>/color/<index currently 1 or 2>
 *   ?interpolation=FADE, etc
 *   ?speed=int color cycle speed. Can be negative
 * /<section>/set
 *   ? brightness 0-255 default -1 no change
 *   ? density= int (default -1 = no change)
 *   ? speed - probably in int default -1 no change
 *   ? effect - name of effect default none no change
 *   ? other parameters specific to the type of effect

 *
 *  Created on: 23 Aug 2025
 *      Author: pmurray
 */

#ifndef SRC_CORE_API_HPP_
#define SRC_CORE_API_HPP_

#include <cstdint>

#import "common.hpp"
#include "json.hpp"
#include "state.hpp"
#include "commands.hpp"

using json = nlohmann::json;

std::pair<int, json> api(const std::string &path, const std::multimap<std::string, std::string> &params, json &command);

#endif /* SRC_CORE_API_HPP_ */
