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

		std::string GetName() const { return this->name; }
		int GetTimesCompleted() const { return this->timesCompleted; }
		virtual bool IsReady() const;

		std::chrono::system_clock::time_point GetLastCompletion() const;
		std::chrono::minutes GetCompletionInterval() const;

		virtual StationResult Complete() = 0;

	protected:
		BaseStation(const std::string name, std::chrono::minutes interval);

		std::chrono::system_clock::time_point lastCompleted;
		std::chrono::minutes completionInterval;

		int timesCompleted;
		const std::string name;
		Status status;

		void SetCompleted();
	};
}