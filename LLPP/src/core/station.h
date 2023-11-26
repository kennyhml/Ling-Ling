#pragma once
#include <chrono>
#include <string>

namespace llpp::core
{

	class LLPPBaseStation
	{
	protected:
		std::chrono::system_clock::time_point lastCompleted;
		std::chrono::duration<int> completionInterval;

		int timesCompleted;
		const std::string name;

		void SetCompleted();

	public:
		LLPPBaseStation(
			const std::string name, std::chrono::duration<int> interval)
			: name(name), completionInterval(interval), timesCompleted(0){};


		const std::chrono::system_clock::time_point GetLastCompletion() const;
		const std::chrono::duration<int> GetCompletionInterval() const;
		const int GetTimesCompleted() const;
		const std::string GetName() const;

		virtual const bool IsReadyForCompletion() const;

		virtual void Complete() = 0;
	};


}