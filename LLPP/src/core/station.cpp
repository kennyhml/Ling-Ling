#include "station.h"

using namespace llpp::core;

const std::chrono::system_clock::time_point
LLPPBaseStation::GetLastCompletion() const
{
	return this->lastCompleted;
}

const std::chrono::seconds LLPPBaseStation::GetCompletionInterval() const
{
	return this->completionInterval;
}

const int LLPPBaseStation::GetTimesCompleted() const
{
	return this->timesCompleted;
}

const std::string LLPPBaseStation::GetName() const { return this->name; }

const bool LLPPBaseStation::IsReady() const
{
	return std::chrono::system_clock::now() - this->lastCompleted >
		   this->completionInterval;
}

void LLPPBaseStation::SetCompleted()
{
	this->timesCompleted++;
	this->lastCompleted = std::chrono::system_clock::now();
}