#pragma once
#include <chrono>
#include <functional>
#include <opencv2/imgcodecs.hpp>



namespace llpp::util
{
	bool Await(const std::function<bool()>& condition,
		std::chrono::milliseconds timeout);

	bool Timedout(const std::chrono::system_clock::time_point& start,
		const std::chrono::milliseconds timeout);

	bool Timedout(const std::chrono::system_clock::time_point& start,
		const std::chrono::seconds timeout);

	const std::string MatToStringBuffer(const cv::Mat& source);

	template <typename Type> const char* GetName(const Type& obj)
	{
		return typeid(obj).name();
	}

	template <typename cast>
	cast GetElapsed(std::chrono::system_clock::time_point start)
	{
		return std::chrono::duration_cast<cast>(
			std::chrono::system_clock::now() - start);
	}

	template <typename cast>
	cast GetTimeLeftUntil(std::chrono::system_clock::time_point timePoint)
	{
		return std::chrono::duration_cast<cast>(
			timePoint - std::chrono::system_clock::now());
	}

	std::string AddNumberToPrefix(
		const std::string& prefix, int number, int fillZeros = 2);
}
