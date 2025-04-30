#include "RichPresence.h"

#include <chrono>
#include <iostream>
#include <print>
#include <thread>

int main(int, char **) {
	using namespace heimskr;

	try {
		std::println("Initializing...");
		richPresence.init();
		std::println("Initialized.");

		bool failed = false;

		if (!richPresence.initActivity([&](discord::Result result) {
			if (result != discord::Result::Ok) {
				failed = true;
			}

			std::println("Activity set.");
		})) {
			std::println(std::cerr, "Failed to initialize rich activity");
			return 1;
		}

		while (true) {
			if (failed) {
				std::println(std::cerr, "Couldn't initialize rich activity");
				return 1;
			}

			richPresence.tick();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	} catch (const std::exception &error) {
		std::println(std::cerr, "{}", error.what());
		return 1;
	}
}
