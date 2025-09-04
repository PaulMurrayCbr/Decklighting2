/*
 * pixelloop.cpp
 *
 *  Created on: 23 Aug 2025
 *      Author: pmurray
 */

#include <atomic>
#include <thread>
#include <iostream>

#include "common.hpp"
#include "state.hpp"
#include "effect.hpp"
#include "pixels.hpp"
#include "pixelloop.hpp"

class PixelLoop {
    std::atomic<bool> running;
    std::thread worker;

    void loop() {
        std::cerr << "Pixel loop started\n";

        int n = 0;
        while (running.load()) {
            GlobalState state;

            inSharedStateMutex([&] {
                state = sharedState;
                sharedState.touched = false;
                sharedState.needsRepaint = false;
                for (SectionState *section = sharedState.section; section < sharedState.section + NSECTIONS; section++) {
                    section->touched = false;
                    section->needsRepaint = false;
                }
            });

            if (state.touched)
                std::cout << "global state touched\n";

            if (state.needsRepaint) {
                std::cout << "global state needs repaint\n";
            }

            for (int i = 0; i < NSECTIONS; i++) {
                if (state.section[i].touched) {
                    std::cout << sectionName(i) << " touched" << '\n';
                }
                if (state.section[i].needsRepaint) {
                    std::cout << sectionName(i) << " needs repaint" << '\n';
                }
            }

            if (state.needsRepaint) {
                set_pixel_global_brightness(state.brightness);
                clear_pixels(0, NPIXELS);

                if (state.on) {
                    for (SectionState *section = state.section; section < sharedState.section + NSECTIONS; section++) {
                        if (section->mode == SectionMode::on) {
                            EFFECT_REPAINT[static_cast<int>(section->effect)](*section);
                        }
                    }
                }

                send_pixels();
            } else if (state.on) {
                bool needs_send = false;

                for (SectionState *section = state.section; section < sharedState.section + NSECTIONS; section++) {
                    if (section->mode == SectionMode::on) {
                        if (section->needsRepaint) {
                            EFFECT_REPAINT[static_cast<int>(section->effect)](*section);
                            needs_send = true;
                        } else {
                            needs_send = EFFECT_ANIMATE[static_cast<int>(section->effect)](*section) || needs_send;
                        }

                    }
                }

                if (needs_send)
                    send_pixels();
            }

            std::this_thread::yield();
        }
        std::cerr << "Pixel loop stopped\n";
    }

public:
    void start() {
        std::cerr << "Starting pixel loop\n";
        running.store(true);
        worker = std::thread(&PixelLoop::loop, this);
    }

    void stop() {
        std::cerr << "Stopping pixel loop\n";
        running.store(false);
        if (worker.joinable())
            worker.join();
    }
};

PixelLoop pixel_loop;

void start_pixelloop() {
    pixel_loop.start();
}

void stop_pixelloop() {
    pixel_loop.stop();
}

