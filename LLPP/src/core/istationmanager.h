#pragma once
#include <chrono>

namespace llpp::core
{
	class IStationManager
	{
	public:
		virtual bool Run() = 0;
		virtual bool IsReadyToRun() = 0;
		virtual std::chrono::minutes GetTimeLeftUntilReady() = 0;
	};
}