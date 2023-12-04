#pragma once
#include <chrono>
#include <memory>
#include <unordered_map>

namespace llpp::core
{
	class BaseStation;

	struct StationResult
	{
		StationResult(BaseStation* station, bool succeeded,
			std::chrono::seconds timeTaken,
			std::unordered_map<std::string, int> obtainedItems)
			: station(station), success(succeeded), completions(completions),
			  timeTaken(timeTaken), obtainedItems(obtainedItems){};

		BaseStation* station;
		const bool success;
		const int completions;
		const std::chrono::seconds timeTaken;
		const std::unordered_map<std::string, int> obtainedItems;
	};
}