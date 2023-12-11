#include "basestation.h"

using namespace llpp::core;


BaseStation::BaseStation(const std::string name, std::chrono::minutes interval)
	: name(name), completionInterval(interval), timesCompleted(0),
	  status(UNKNOWN){};

std::chrono::system_clock::time_point BaseStation::GetLastCompletion() const
{
	return this->lastCompleted;
}

std::chrono::system_clock::time_point BaseStation::GetLastCompletion() const
{
	return this->lastCompleted + completionInterval;
}

std::chrono::minutes BaseStation::GetCompletionInterval() const
{
	return this->completionInterval;
}

bool BaseStation::IsReady() const
{
	return (std::chrono::system_clock::now() - this->lastCompleted) >
		   this->completionInterval;
}

void BaseStation::SetCompleted()
{
	this->timesCompleted++;
	this->lastCompleted = std::chrono::system_clock::now();
}