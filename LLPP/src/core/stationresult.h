#pragma once
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

namespace llpp::core
{
	class BaseStation;

	struct StationResult
	{
		StationResult(BaseStation* t_station, bool t_succeeded,
			std::chrono::seconds t_time_taken,
			std::unordered_map<std::string, int> t_obtained_items)
			: station(t_station), success(t_succeeded),
			  time_taken(t_time_taken), obtained_items(t_obtained_items){};

		BaseStation* station;
		const bool success;
		const std::chrono::seconds time_taken;
		const std::unordered_map<std::string, int> obtained_items;
	};
}