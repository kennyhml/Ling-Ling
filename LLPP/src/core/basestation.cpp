#include "basestation.h"

namespace llpp::core
{
	BaseStation::BaseStation(
		const std::string name, std::chrono::minutes interval)
		: name(name), completion_interval(interval), times_completed(0),
		  status(UNKNOWN){};

	std::chrono::system_clock::time_point
	BaseStation::get_last_completion() const
	{
		return last_completed;
	}

	std::chrono::system_clock::time_point
	BaseStation::get_next_completion() const
	{
		return last_completed + completion_interval;
	}

	std::chrono::minutes BaseStation::get_completion_interval() const
	{
		return completion_interval;
	}

	bool BaseStation::is_ready() const
	{
		return (std::chrono::system_clock::now() - last_completed) >
			   completion_interval;
	}

	void BaseStation::set_completed()
	{
		times_completed++;
		last_completed = std::chrono::system_clock::now();
	}
}
