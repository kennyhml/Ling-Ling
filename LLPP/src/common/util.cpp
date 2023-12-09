#include "util.h"
#include <iomanip>
#include <sstream>
#include <string>

bool llpp::util::Await(
	const std::function<bool()>& condition, std::chrono::milliseconds timeout)
{
	auto start_time = std::chrono::system_clock::now();
	while (!condition()) {
		auto current_time = std::chrono::system_clock::now();
		if (Timedout(start_time, timeout)) {
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	return true;
}

bool llpp::util::Timedout(const std::chrono::system_clock::time_point& start,
	const std::chrono::milliseconds timeout)
{
	auto now = std::chrono::system_clock::now();
	auto timePassed = now - start;

	return timePassed >= timeout;
}

bool llpp::util::Timedout(const std::chrono::system_clock::time_point& start,
	const std::chrono::seconds timeout)
{
	return Timedout(
		start, std::chrono::duration_cast<std::chrono::milliseconds>(timeout));
}

const std::string llpp::util::MatToStringBuffer(const cv::Mat& source)
{
	std::vector<uchar> imageBuffer;
	cv::imencode(".png", source, imageBuffer);
	std::string imageContent(imageBuffer.begin(), imageBuffer.end());
	return imageContent;
}

std::string llpp::util::AddNumberToPrefix(
	const std::string& prefix, int number, int fillZeros)
{
	std::ostringstream oss;
	oss << std::setw(fillZeros) << std::setfill('0') << number;
	return prefix + oss.str();
}