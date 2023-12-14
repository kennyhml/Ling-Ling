#pragma once
#include <chrono>
#include <functional>
#include <opencv2/imgcodecs.hpp>

namespace llpp::util
{
	bool await(const std::function<bool()>& condition,
		std::chrono::milliseconds timeout);

	bool timedout(const std::chrono::system_clock::time_point& start,
		const std::chrono::milliseconds timeout);

	bool timedout(const std::chrono::system_clock::time_point& start,
		const std::chrono::seconds timeout);

	bool timedout(const std::chrono::system_clock::time_point& start,
		const std::chrono::minutes timeout);

	const std::string mat_to_strbuffer(const cv::Mat& source);


	template <typename cast>
	cast get_elapsed(std::chrono::system_clock::time_point start)
	{
		return std::chrono::duration_cast<cast>(
			std::chrono::system_clock::now() - start);
	}

	template <typename cast>
	cast get_time_left_until(std::chrono::system_clock::time_point timePoint)
	{
		return std::chrono::duration_cast<cast>(
			timePoint - std::chrono::system_clock::now());
	}

	std::string add_num_to_prefix(
		const std::string& prefix, int number, int fillZeros = 2);
}
