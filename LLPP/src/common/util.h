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



}
