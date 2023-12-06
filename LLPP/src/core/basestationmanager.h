#pragma once
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace llpp::core
{
	class BaseStationManager
	{
	public:
		virtual bool CompleteReadyStations() = 0;
		virtual bool IsReadyToRun() = 0;
		virtual std::chrono::minutes GetTimeLeftUntilReady() = 0;

	protected:
		BaseStationManager(std::string stationPrefix, int numberOfStations);

		const std::string stationPrefix;
		const int numberOfStations;

		std::string CreateStationName(std::string prefix, int number);
	};
}