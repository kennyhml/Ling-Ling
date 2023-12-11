#pragma once
#include <chrono>

namespace llpp::core
{
	class IStationManager
	{
	public:
		virtual bool Run() = 0;
		virtual bool IsReadyToRun() const = 0;
		virtual std::chrono::minutes GetTimeLeftUntilReady() const = 0;
	};
}