#include "util.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace llpp::util
{
    bool await(const std::function<bool()>& condition, std::chrono::milliseconds timeout)
    {
        auto start_time = std::chrono::system_clock::now();
        while (!condition()) {
            auto current_time = std::chrono::system_clock::now();
            if (timedout(start_time, timeout)) { return false; }
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
        return timedout(
            start, std::chrono::duration_cast<std::chrono::milliseconds>(timeout));
    }

    bool timedout(const std::chrono::system_clock::time_point& start,
                  const std::chrono::minutes timeout)
    {
        return timedout(
            start, std::chrono::duration_cast<std::chrono::milliseconds>(timeout));
    }

    std::string add_num_to_prefix(const std::string& prefix, int number, int fill_zeros)
    {
        std::ostringstream oss;
        oss << std::setw(fill_zeros) << std::setfill('0') << number;
        return prefix + oss.str();
    }

    std::string time_to_json(const std::chrono::system_clock::time_point& time)
    {
        std::stringstream ss;
        ss << std::chrono::system_clock::to_time_t(time);
        return ss.str();
    }

    std::chrono::system_clock::time_point json_to_time(const std::string& json)
    {
        std::stringstream ss(json);
        std::time_t unix;
        ss >> unix;
        return std::chrono::system_clock::from_time_t(unix);
    }

    int64_t time_t_now()
    {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }


    cv::Mat bytes_to_mat(const uchar* data, const int width, const int height,
                         const int channels)
    {
        return cv::Mat(height, width, CV_8UC(channels), (uchar*) data).clone();
    }

    std::string make_counter(const std::chrono::system_clock::duration duration)
    {
        using namespace std::chrono;
        using namespace std::chrono_literals;
        std::ostringstream oss;

        const auto d = duration_cast<days>(duration);
        const auto h = duration_cast<hours>(duration % 24h);
        const auto m = duration_cast<minutes>(duration % 60min);
        const auto s = duration_cast<seconds>(duration % 60s);

        if (d.count() > 0) {
            oss << std::setw(2) << std::setfill('0') << d.count() << ":";
        }
        if (h.count() > 0 || d.count() > 0) {
            oss << std::setw(2) << std::setfill('0') << h.count() << ":";
        }
        if (m.count() > 0 || h.count() > 0 || d.count() > 0) {
            oss << std::setw(2) << std::setfill('0') << m.count() << ":";
        }
        oss << std::setw(2) << std::setfill('0') << s.count();
        return oss.str().size() <= 2 ? "-:-:-" : oss.str();
    }

    std::string lowercase(const std::string& string)
    {
        std::string result = string;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
}
