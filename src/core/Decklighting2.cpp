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

using json = nlohmann::json;

static std::atomic<bool> g_stop{false};
void handle_signal(int) { g_stop.store(true); }

int main() {
    // === Config ===
//    const int GPIO_PIN = 18;      // PWM0 (requires level shifter to 5V on data)
//    const int LED_COUNT = 60;

	start_webserver();

    const uint16_t PORT = 8080;
    const std::string static_dir = "web";

    std::cerr << "Using web files at " << fs::canonical(static_dir) << "\n";


    // HTTP
    httplib::Server svr;

    // Serve your React build
    if (fs::exists(static_dir)) {
        svr.set_mount_point("/", static_dir.c_str());
        // SPA fallback: serve index.html for unknown routes (but not under /api)
        svr.set_file_extension_and_mimetype_mapping("js", "text/javascript");
        svr.set_file_extension_and_mimetype_mapping("css", "text/css");

//        svr.set_not_found_handler([&](const httplib::Request& req, httplib::Response& res) {
//            if (req.path.rfind("/api/", 0) == 0) {
//                res.status = 404;
//                res.set_content("Not found", "text/plain");
//            } else {
//                std::string index_path = static_dir + "/index.html";
//                std::ifstream f(index_path, std::ios::binary);
//                if (f) {
//                    std::stringstream ss; ss << f.rdbuf();
//                    res.set_content(ss.str(), "text/html");
//                } else {
//                    res.status = 404;
//                    res.set_content("index.html not found", "text/plain");
//                }
//            }
//        });
    } else {
        std::cerr << "Warning: static dir '" << static_dir << "' not found; only /api/* will work.\n";
    }

    // Simple JSON helpers
    auto json_ok = [](const json& j) {
        httplib::Response r;
        r.status = 200;
        r.set_content(j.dump(), "application/json");
        return r;
    };
    auto json_err = [](int code, const std::string& msg) {
        httplib::Response r;
        r.status = code;
        r.set_content(json{{"error", msg}}.dump(), "application/json");
        return r;
    };

    // GET /api/status
    svr.Get("/api/status", [&](const httplib::Request&, httplib::Response& res) {
//        auto c = pixels.current();
        json j = {
//            {"mode", (c.mode == Mode::Off ? "off" : "solid")},
//            {"r", c.r}, {"g", c.g}, {"b", c.b},
//            {"brightness", c.brightness},
//            {"led_count", pixels.ledCount()}
        };
        res = json_ok(j);
    });

    // POST /api/solid  { r,g,b, brightness? }
    svr.Post("/api/solid", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.get_header_value("Content-Type").find("application/json") == std::string::npos) {
            res = json_err(415, "Content-Type must be application/json");
            return;
        }
        try {
            auto j = json::parse(req.body);
            uint8_t r = j.value("r", 0);
            uint8_t g = j.value("g", 0);
            uint8_t b = j.value("b", 0);
            uint8_t brightness = j.value("brightness", 255);
 //           pixels.setSolid(r, g, b, brightness);
            res = json_ok({{"ok", true}});
        } catch (...) {
            res = json_err(400, "Invalid JSON");
        }
    });

    // POST /api/off
    svr.Post("/api/off", [&](const httplib::Request&, httplib::Response& res) {
//       pixels.setOff();
        res = json_ok({{"ok", true}});
    });

    // Graceful shutdown
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::thread http_thread([&]{
        std::cout << "HTTP on http://0.0.0.0:" << PORT << "\n";
        svr.listen("0.0.0.0", PORT);
    });

    // Poll for stop signal
    while (!g_stop.load()) std::this_thread::sleep_for(std::chrono::milliseconds(200));

    svr.stop();
    if (http_thread.joinable()) http_thread.join();
//    pixels.stop();
    std::cout << "Stopped.\n";
    return 0;
}
