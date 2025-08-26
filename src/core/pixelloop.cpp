/*
 * pixelloop.cpp
 *
 *  Created on: 23 Aug 2025
 *      Author: pmurray
 */

#include <atomic>
#include <thread>
#include <iostream>

#include "state.hpp"
#include "pixelloop.hpp"

class PixelLoop {
	std::atomic<bool> running;
	std::thread worker;

	void loop() {
		std::cerr << "Pixel loop started\n";

		int n=0;
		while (running.load()) {
			if(n++ < 10) {
				std::cerr << "Pixel loop running\n";
			}

			GlobalState state;

			inSharedStateMutex([&] {
				state = sharedState;
				sharedState.touched = false;
				sharedState.needsReset = false;
				for (SectionState *section = sharedState.section; section < sharedState.section + NSECTIONS;  section++) {
					section->touched = false;
					section->needsReset = false;
				}
			});

			if(state.touched) std::cout << "global state touched";
					if(state.needsReset) std::cout << "global state needs reset";

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

/*


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

 #define CPPHTTPLIB_OPENSSL_SUPPORT
 #include "httplib.h"
 #include <nlohmann/json.hpp>
 using json = nlohmann::json;

 extern "C" {
 #include "rpi_ws281x/ws2811.h"
 }

 // ===== NeoPixel controller (rpi_ws281x) =====
 enum class Mode { Off, Solid };

 struct Command {
 Mode mode{Mode::Off};
 uint8_t r{0}, g{0}, b{0};
 uint8_t brightness{255};
 };

 class PixelController {
 public:
 PixelController(int gpio_pin, int led_count, int dma=10, uint8_t brightness=128)
 : led_count_(led_count) {
 // Configure ws2811
 ledstring_.freq = WS2811_TARGET_FREQ;
 ledstring_.dmanum = dma;
 ledstring_.channel[0].gpionum = gpio_pin;
 ledstring_.channel[0].count = led_count_;
 ledstring_.channel[0].invert = 0;
 ledstring_.channel[0].brightness = brightness;
 ledstring_.channel[0].strip_type = WS2811_STRIP_GRB; // adjust if needed
 ledstring_.channel[1].gpionum = 0;
 ledstring_.channel[1].count = 0;
 running_.store(false);
 }

 bool init() {
 auto st = ws2811_init(&ledstring_);
 if (st != WS2811_SUCCESS) {
 std::cerr << "ws2811_init failed: " << ws2811_get_return_t_str(st) << "\n";
 return false;
 }
 running_.store(true);
 worker_ = std::thread(&PixelController::loop, this);
 return true;
 }

 void stop() {
 running_.store(false);
 {
 std::lock_guard<std::mutex> lk(mu_);
 pending_.reset(); // wake worker even if no command
 }
 cv_.notify_all();
 if (worker_.joinable()) worker_.join();
 clear_();
 ws2811_fini(&ledstring_);
 }

 void setSolid(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
 Command c;
 c.mode = Mode::Solid; c.r = r; c.g = g; c.b = b; c.brightness = brightness;
 push_(c);
 }

 void setOff() {
 Command c; c.mode = Mode::Off; c.brightness = 0;
 push_(c);
 }

 // Read-only snapshot for /api/status
 Command current() const {
 std::lock_guard<std::mutex> lk(mu_);
 return current_;
 }

 int ledCount() const { return led_count_; }

 private:
 void push_(const Command& c) {
 {
 std::lock_guard<std::mutex> lk(mu_);
 pending_ = c;
 }
 cv_.notify_one();
 }

 void apply_(const Command& c) {
 // Update brightness first
 ledstring_.channel[0].brightness = c.brightness;
 if (c.mode == Mode::Off) {
 for (int i = 0; i < led_count_; ++i) ledstring_.channel[0].leds[i] = 0x000000;
 } else { // Solid
 uint32_t packed = ((uint32_t)c.g << 16) | ((uint32_t)c.r << 8) | ((uint32_t)c.b);
 for (int i = 0; i < led_count_; ++i) ledstring_.channel[0].leds[i] = packed;
 }
 ws2811_render(&ledstring_);
 }

 void clear_() {
 for (int i = 0; i < led_count_; ++i) ledstring_.channel[0].leds[i] = 0;
 ws2811_render(&ledstring_);
 }

 void loop() {
 // Event-driven: wait for commands; no busy-wait
 while (running_.load()) {
 std::unique_lock<std::mutex> lk(mu_);
 cv_.wait(lk, [&]{ return !running_.load() || pending_.has_value(); });
 if (!running_.load()) break;
 current_ = *pending_;
 pending_.reset();
 Command to_apply = current_;
 lk.unlock();
 apply_(to_apply);
 }
 }

 ws2811_t ledstring_ {};
 int led_count_;
 std::atomic<bool> running_;
 std::thread worker_;
 mutable std::mutex mu_;
 std::condition_variable cv_;
 std::optional<Command> pending_;
 Command current_; // last applied state
 };

 */
