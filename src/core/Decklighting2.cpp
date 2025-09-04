// main.cpp
#include <atomic>
#include <thread>
#include <csignal>
#include <iostream>

#include "common.hpp"

#include "webserver.hpp"
#include "state.hpp"
#include "pixelloop.hpp"

#ifdef __APPLE__
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

static std::atomic<bool> g_stop { false };
void handle_signal(int) {
    g_stop.store(true);
}

int main() {
    // === Config ===
//    const int GPIO_PIN = 18;      // PWM0 (requires level shifter to 5V on data)
//    const int LED_COUNT = 60;

    std::cout << "Starting ...\n";

    // initialise the sections
    int at = 0;
    // we go backwards, because I prefer to think of the door as "first", but the pixels are injected into the other end
    // this means all the effects will be mirrored, but meh.
    for (int i = NSECTIONS - 1; i >= 0; i--) {
        sharedState.section[i].mode = SectionMode::on;
        sharedState.section[i].touched = true;
        sharedState.section[i].needsRepaint = true;
    }
    sharedState.touched = true;
    sharedState.needsRepaint = true;

    recompute_sections();

    start_webserver();
    start_pixelloop();

    // Graceful shutdown
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::cout << "Started ...\n";

    // Poll for stop signal
    while (!g_stop.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Stopping ...\n";

    stop_pixelloop();
    stop_webserver();

//    pixels.stop();
    std::cout << "Stopped.\n";
    return 0;
}
