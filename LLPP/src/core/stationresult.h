#pragma once
#include <chrono>
#include <memory>
#include <unordered_map>

namespace llpp::core
{
	class LLPPBaseStation;

	struct StationResult
	{
		StationResult(LLPPBaseStation* station, bool succeeded, int completions,
			std::chrono::seconds timeTaken,
			std::unordered_map<std::string, int> obtainedItems)
			: station(station), success(succeeded), completions(completions),
			  timeTaken(timeTaken), obtainedItems(obtainedItems){};

		LLPPBaseStation* station;
		const bool success;
		const int completions;
		const std::chrono::seconds timeTaken;
		std::unordered_map<std::string, int> obtainedItems;
	};
}