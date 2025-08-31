// main.cpp
#include <atomic>
#include <thread>
#include <csignal>
#include <iostream>

#import "common.hpp"

#include "webserver.hpp"
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

    for (auto const &pair : EFFECT_TYPE_NAME_OF) {
        std::cout << static_cast<int>(pair.first) << " → " << pair.second << "\n";
    }
    for (auto const &pair : EFFECT_TYPE_ENUM_OF) {
        std::cout << pair.first << " → " << static_cast<int>(pair.second) << "\n";
    }
    for (auto const &pair : INTERPOLATION_TYPE_NAME_OF) {
        std::cout << static_cast<int>(pair.first) << " → " << pair.second << "\n";
    }
    for (auto const &pair : INTERPOLATION_TYPE_ENUM_OF) {
        std::cout << pair.first << " → " << static_cast<int>(pair.second) << "\n";
    }
    for (auto const &pair : SECTION_NAME_OF) {
        std::cout << static_cast<int>(pair.first) << " → " << pair.second << "\n";
    }
    for (auto const &pair : SECTION_ENUM_OF) {
        std::cout << pair.first << " → " << static_cast<int>(pair.second) << "\n";
    }

    std::cout << "Starting ...\n";

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
