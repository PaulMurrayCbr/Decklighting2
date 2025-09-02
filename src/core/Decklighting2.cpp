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
    for (int i = 0; i < NSECTIONS; i++) {
        SECTION_START[i] = at;
        at += SECTION_LEN[i];
        sharedState.section[i].length = SECTION_LEN[i];
        sharedState.section[i].mode = SectionMode::on;
    }

    start_webserver();
    start_pixelloop();

    // Graceful shutdown
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::cout << "Started ...\n";

    // Poll for stop signal
    while (!g_stop.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Stopping ...\n";

    stop_pixelloop();
    stop_webserver();

//    pixels.stop();
    std::cout << "Stopped.\n";
    return 0;
}
