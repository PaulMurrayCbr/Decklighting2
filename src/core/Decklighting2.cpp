// main.cpp
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include <csignal>
#include <iostream>

#include "httplib.h"
#include "json.hpp"

#include "state.hpp"
#include "webserver.hpp"

#ifdef __APPLE__
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

static std::atomic<bool> g_stop{false};
void handle_signal(int) { g_stop.store(true); }

int main() {
    // === Config ===
//    const int GPIO_PIN = 18;      // PWM0 (requires level shifter to 5V on data)
//    const int LED_COUNT = 60;

	start_webserver();

    // Graceful shutdown
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    // Poll for stop signal
    while (!g_stop.load()) std::this_thread::sleep_for(std::chrono::milliseconds(200));

    stop_webserver();

//    pixels.stop();
    std::cout << "Stopped.\n";
    return 0;
}
