#include "RichPresence.h"
#include "Utils.h"

#include <cstdlib>
#include <format>

namespace heimskr {
	Discord::~Discord() {
		reset();
	}

	uint64_t getApplicationID() {
		const char *token_string = std::getenv("DISCORD_APP_ID");
		if (!token_string) {
			throw std::runtime_error("Please set DISCORD_APP_ID.");
		}

		return parseNumber<uint64_t>(token_string);
	}

	bool Discord::init(discord::Result *result_out) {
		discord::Core *core_raw{};
		const discord::Result result = discord::Core::Create(getApplicationID(), DiscordCreateFlags_Default, &core_raw);

		if (result_out != nullptr) {
			*result_out = result;
		}

		if (result != discord::Result::Ok) {
			throw std::runtime_error(std::format("Couldn't initialize Discord: {}", static_cast<int>(result)));
			return false;
		}

		core.reset(core_raw);
		return true;
	}

	std::function<void(discord::Result)> Discord::makeActivityCallback(std::function<void(discord::Result)> callback) const {
		return [callback = std::move(callback)](discord::Result result) {
			if (result != discord::Result::Ok) {
				if (result == discord::Result::TransactionAborted) {
					throw std::runtime_error("Couldn't set activity: transaction aborted");
				} else {
					throw std::runtime_error(std::format("Couldn't set activity ({})", static_cast<int>(result)));
				}
			}
			if (callback) {
				callback(result);
			}
		};
	}

	bool Discord::updateActivity(std::function<void(discord::Result)> callback) {
		if (!core) {
			return false;
		}

		core->ActivityManager().UpdateActivity(activity, makeActivityCallback(std::move(callback)));
		return true;
	}

	bool Discord::initActivity(std::function<void(discord::Result)> callback) {
		if (!core) {
			return false;
		}

		details = defaultDetails();
		activity.SetDetails(details.c_str());
		auto &assets = activity.GetAssets();
		if (const char *image = std::getenv("DISCORD_ACTIVITY_IMAGE")) {
			assets.SetLargeImage(image);
		}
		if (const char *text = std::getenv("DISCORD_ACTIVITY_IMAGE_TEXT")) {
			assets.SetLargeText(text);
		}
		updateActivity(std::move(callback));
		return true;
	}

	bool Discord::tick() {
		if (!core) {
			return false;
		}
		core->RunCallbacks();
		return true;
	}

	bool Discord::setActivityDetails(const char *new_details, bool update) {
		details = new_details;
		activity.SetDetails(new_details);
		return update? updateActivity() : true;
	}

	bool Discord::setActivityDetails(std::string new_details, bool update) {
		details = std::move(new_details);
		activity.SetDetails(details.c_str());
		return update? updateActivity() : true;
	}

	bool Discord::setActivityStartTime(std::chrono::system_clock::time_point when, bool update) {
		startingTime = when;
		activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(when.time_since_epoch()).count());
		return update? updateActivity() : true;
	}

	bool Discord::setActivityStartTime(bool update) {
		return setActivityStartTime(std::chrono::system_clock::now(), update);
	}

	const std::string & Discord::getDetails() const {
		return details;
	}

	void Discord::reset() {
		if (!core) {
			return;
		}
		details = defaultDetails();
		core.reset();
	}

	std::string Discord::defaultDetails() {
		if (const char *details = std::getenv("DISCORD_ACTIVITY_DETAILS")) {
			return details;
		}
		return "Being a mystery";
	}

	Discord richPresence;
}
