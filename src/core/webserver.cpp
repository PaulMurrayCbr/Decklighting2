#include <cstdint>
#include <filesystem>
#include <thread>
#include <iostream>
#include <atomic>
#include <unistd.h>

#include "httplib.h"
#include "json.hpp"

#include "webserver.hpp"
#include "api.hpp"

#ifdef __APPLE__
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

using json = nlohmann::json;

class Webserver {
	std::atomic<int> request_serial { 0 };

	httplib::Server svr;

	std::thread http_thread;

	httplib::Response json_response(std::pair<int, json> api_result) {

		json result;
		result["version"] = "0.1";
		result["instance"] = getpid();
		result["serial"] = ++request_serial;
		result["status"] = api_result.first;
		result["result"] = api_result.second;
		result["server-error"] = false;

		httplib::Response r;
		r.status = api_result.first;
		r.set_content(result.dump(), "application/json");
		return r;
	}
	;

public:
	void start() {

		const uint16_t PORT = 8080;
		const std::string static_dir = "web";

		std::cerr << "Using web files at " << fs::canonical(static_dir) << "\n";

		// Serve your React build
		if (fs::exists(static_dir)) {
			svr.set_mount_point("/", static_dir.c_str());
			// SPA fallback: serve index.html for unknown routes (but not under /api)
			svr.set_file_extension_and_mimetype_mapping("js",
					"text/javascript");
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
			std::cerr << "Warning: static dir '" << static_dir
					<< "' not found; only /api/* will work.\n";
		}

		svr.Get("/api/.*",
				[&](const httplib::Request &req, httplib::Response &res) {
					json request_json = json::object();
					std::pair<int, json> api_response = api(req.path, req.params, request_json);
					res = json_response(api_response);
				});

		// POST /api/
		svr.Post("/api/.*",
				[&](const httplib::Request &req, httplib::Response &res) {
					json request_json = json::parse(req.body);
					std::pair<int, json> api_response = api(req.path, req.params, request_json);
					res = json_response(api_response);
				});

		http_thread = std::thread([&] {
			std::cout << "HTTP on http://0.0.0.0:" << PORT << "\n";
			svr.listen("0.0.0.0", PORT);
		});

	}

	void stop() {
		svr.stop();
		if (http_thread.joinable())
			http_thread.join();
	}
};

Webserver webserver;

void start_webserver() {
	webserver.start();
}

void stop_webserver() {
	webserver.stop();
}
