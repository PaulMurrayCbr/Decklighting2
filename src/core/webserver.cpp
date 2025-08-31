#include <cstdint>
#include <filesystem>
#include <thread>
#include <iostream>
#include <atomic>
#include <unistd.h>

#include "httplib.h"
#include "json.hpp"

#import "common.hpp"
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
		result["application"] = "Decklighting 2";
		result["version"] = "0.1";
		result["instance"] = getpid();
		result["serial"] = ++request_serial;
		result["status"] = api_result.first;
		result["result"] = api_result.second;

		httplib::Response r;
		r.status = api_result.first;
		r.set_content(result.dump(), "application/json");
		return r;
	};


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

		} else {
			std::cerr << "Warning: static dir '" << static_dir
					<< "' not found; only /api/* will work.\n";
		}

		svr.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
		    // Check if the path starts with the static mount point
		    if (res.status == 404 && req.path != "/api" && req.path.rfind("/api/", 0) != 0) { // doesn't start with /api
		    	res.set_content("<html><body>" + std::to_string(res.status) + ": <tt>" + req.path + "</tt> Not Found.</body></html>", "text/html");
		    }
		    // else: let normal routes or default 404 happen
		});

		auto doGet  = [&](const httplib::Request &req, httplib::Response &res) {
			json request_json = json::object();
			std::pair<int, json> api_response = api(req.path, req.params, request_json);
			res = json_response(api_response);
		};

		auto doPost = [&](const httplib::Request &req, httplib::Response &res) {
			json request_json = json::parse(req.body);
			std::pair<int, json> api_response = api(req.path, req.params, request_json);
			res = json_response(api_response);
		};

		svr.Get("/api", doGet);
		svr.Get("/api/.*", doGet);
		svr.Post("/api", doPost);
		svr.Post("/api/.*", doPost);

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
