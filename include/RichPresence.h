#pragma once

#include <cstdint>
#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "discord.h"

namespace heimskr {
	class Discord {
		public:
			Discord() = default;
			~Discord();

			bool init(discord::Result *result_out = nullptr);
			bool initActivity(std::function<void(discord::Result)> callback = {});
			bool tick();
			bool setActivityDetails(const char *, bool update = true);
			bool setActivityDetails(std::string, bool update = true);
			bool setActivityStartTime(std::chrono::system_clock::time_point = std::chrono::system_clock::now(), bool update = true);
			bool setActivityStartTime(bool update = true);
			const std::string & getDetails() const;
			void reset();

		private:
			std::unique_ptr<discord::Core> core;
			discord::Activity activity{};
			std::string details;
			std::chrono::system_clock::time_point startingTime;

			std::function<void(discord::Result)> makeActivityCallback(std::function<void(discord::Result)> = {}) const;
			bool updateActivity(std::function<void(discord::Result)> = {});
			static std::string defaultDetails();
	};

	extern Discord richPresence;
}
