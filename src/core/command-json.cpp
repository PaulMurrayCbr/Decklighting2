/*
 * commands.hpp
 *
 *  Created on: 10 Oct 2025
 *      Author: pmurray
 */

#include "../lib/optional.hpp"
#include "../lib/json.hpp"
using json = nlohmann::json;

#include "common.hpp"
#include "state.hpp"
#include "commands.hpp"
#include "command-json.hpp"

// TODO: the state must be read IN MUTEX

namespace {
    json local_getSectionJson(Section section) {
        // todo read state in a mutex and save it as json

        json state = { };

        return state;
    }

    void local_applySectionJson(Section section, json load) {
        // TODO: work out what commands to build, and send them to commands
    }
}

json getSectionJson(Section section) {
    return local_getSectionJson(section);
}

void applySectionJson(Section section, json load) {
    local_applySectionJson(section, load);
}
