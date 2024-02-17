#include "util.h"
#include <iomanip>
#include <sstream>
#include <string>

namespace llpp::util
{
	bool await(const std::function<bool()>& condition,
		std::chrono::milliseconds timeout)
	{
		auto start_time = std::chrono::system_clock::now();
		while (!condition()) {
			auto current_time = std::chrono::system_clock::now();
			if (timedout(start_time, timeout)) {
				return false;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		return true;
	}

	bool timedout(const std::chrono::system_clock::time_point& start,
		const std::chrono::milliseconds timeout)
	{
		auto now = std::chrono::system_clock::now();
		auto time_passed = now - start;

		return time_passed >= timeout;
	}

	bool timedout(const std::chrono::system_clock::time_point& start,
		const std::chrono::seconds timeout)
	{
		return timedout(start,
			std::chrono::duration_cast<std::chrono::milliseconds>(timeout));
	}

	bool timedout(const std::chrono::system_clock::time_point& start,
		const std::chrono::minutes timeout)
	{
		return timedout(start,
			std::chrono::duration_cast<std::chrono::milliseconds>(timeout));
	}

	std::string add_num_to_prefix(
		const std::string& prefix, int number, int fill_zeros)
	{
		std::ostringstream oss;
		oss << std::setw(fill_zeros) << std::setfill('0') << number;
		return prefix + oss.str();
	}

	std::string get_role_mention(const std::string& role)
	{
		return std::format("<@&{}>", role);
	}

	std::string get_user_mention(const std::string& zser)
	{
		return std::format("<@{}>", zser);
	}


}
