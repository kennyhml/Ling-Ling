#pragma once
#include <chrono>

namespace llpp::core
{
	class IStationManager
	{
	public:
		virtual bool run() = 0;
		virtual bool is_ready_to_run() const = 0;
		virtual std::chrono::minutes get_time_left_until_ready() const = 0;
	};
}