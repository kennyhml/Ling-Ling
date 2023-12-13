#pragma once
#include "stationresult.h"
#include <chrono>
#include <string>

namespace llpp::core
{
	class BaseStation
	{
	public:
		enum Status
		{
			UNKNOWN,
			IN_COMPLETION,
			ON_COOLDOWN,
			PENDING,
			DISABLED
		};

		std::string get_name() const { return name; }
		int get_times_completed() const { return times_completed; }
		virtual bool is_ready() const;

		std::chrono::system_clock::time_point get_last_completion() const;
		std::chrono::system_clock::time_point get_next_completion() const;
		std::chrono::minutes get_completion_interval() const;

		virtual StationResult complete() = 0;

	protected:
		BaseStation(const std::string name, std::chrono::minutes interval);

		std::chrono::system_clock::time_point last_completed;
		std::chrono::minutes completion_interval;

		int times_completed;
		const std::string name;

		Status status;

		void set_completed();
	};
}