/*
 * commands.cpp
 *
 *  Created on: 31 Aug 2025
 *      Author: pmurray
 */

#import "common.hpp"
#import "commands.hpp"

void handleCommand(const GlobalOnCommand &cmd) {
    inSharedStateMutex([&] {
        sharedState.on = true;
        if (cmd.brightness != -1) {
            sharedState.brightness = cmd.brightness;
        }
        sharedState.touched = true;
        sharedState.needsRepaint = true;
    });
}

void handleCommand(const GlobalOffCommand &cmd) {
    inSharedStateMutex([&] {
        sharedState.on = false;
        sharedState.touched = true;
        sharedState.needsRepaint = true;
    });
}
