#pragma once
#include "stationresult.h"
#include <chrono>
#include <string>

namespace llpp::core
{
	class LLPPBaseStation
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

		const std::chrono::system_clock::time_point GetLastCompletion() const;
		const std::chrono::minutes GetCompletionInterval() const;
		const int GetTimesCompleted() const;
		const std::string GetName() const;

		virtual const bool IsReady() const;

		virtual const StationResult Complete() = 0;

	protected:
		LLPPBaseStation(const std::string name, std::chrono::minutes interval)
			: name(name), completionInterval(interval), timesCompleted(0),
			  status(UNKNOWN){};

		std::chrono::system_clock::time_point lastCompleted;
		std::chrono::minutes completionInterval;

		int timesCompleted;
		const std::string name;
		Status status;

		void SetCompleted();
	};
}