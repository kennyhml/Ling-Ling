#pragma once
#include <chrono>
#include <functional>
#include <opencv2/imgcodecs.hpp>

namespace llpp::util
{
	bool await(const std::function<bool()>& condition,
		std::chrono::milliseconds timeout);

	bool timedout(const std::chrono::system_clock::time_point& start,
		std::chrono::milliseconds timeout);

	bool timedout(const std::chrono::system_clock::time_point& start,
		std::chrono::seconds timeout);

	bool timedout(const std::chrono::system_clock::time_point& start,
		std::chrono::minutes timeout);

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

	std::string get_role_mention(const std::string& role);
	std::string get_user_mention(const std::string& user);

}
