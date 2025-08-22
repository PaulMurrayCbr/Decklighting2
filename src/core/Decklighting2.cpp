//============================================================================
// Name        : Decklighting2.cpp
// Author      : pmurray at bigpond.com
// Version     :
// Copyright   : Paul Murray 2025
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

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

// #define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

#include "pixels.hpp"

using namespace std;

int main() {
	cout << helloWorld() << endl; // prints !!!Hello World!!!
	return 0;
}
