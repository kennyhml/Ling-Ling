#pragma once
#include <chrono>
#include <dpp/message.h>
#include <memory>
#include <unordered_map>

namespace llpp::core
{
	struct StationResult
	{
		StationResult(bool succeeded, std::chrono::seconds timeTaken,
			std::unordered_map<std::string, int> obtainedItems,
			dpp::embed* embed = nullptr)
			: success(succeeded), timeTaken(timeTaken),
			  obtainedItems(obtainedItems), discordEmbed(embed){};

		bool success;
		std::chrono::duration<std::chrono::seconds> timeTaken;
		std::unordered_map<std::string, int> obtainedItems;
		std::shared_ptr<dpp::embed> discordEmbed;
	};

}