#include "basestationmanager.h"

using namespace llpp::core;

BaseStationManager::BaseStationManager(
	std::string stationPrefix, int numberOfStations)
	: stationPrefix(stationPrefix), numberOfStations(numberOfStations){};

std::string BaseStationManager::CreateStationName(
	std::string prefix, int number)
{
	std::ostringstream oss;
	oss << std::setw(2) << std::setfill('0') << number;
	return prefix + oss.str();
}