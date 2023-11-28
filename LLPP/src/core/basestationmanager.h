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
		virtual const bool CompleteReadyStations() = 0;
		virtual const bool IsReadyToRun() = 0;
		virtual std::chrono::minutes GetTimeLeftUntilReady() = 0;

	protected:
		BaseStationManager(std::string stationPrefix, int numberOfStations)
			: stationPrefix(stationPrefix),
			  numberOfStations(numberOfStations){};

		std::string stationPrefix;
		int numberOfStations;


		std::string CreateStationName(const std::string& prefix, int number)
		{
			std::ostringstream oss;
			oss << std::setw(2) << std::setfill('0') << number;
			return prefix + oss.str();
		}
	};



}