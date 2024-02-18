#include "helpers.h"
#include <opencv2/imgcodecs.hpp>

namespace llpp::discord
{
    std::string strbuf(const cv::Mat& source)
    {
        std::vector<uchar> img_buffer;
        cv::imencode(".png", source, img_buffer);
        std::string img_content(img_buffer.begin(), img_buffer.end());
        return img_content;
    }

    void set_now_timestamp(dpp::embed& embed)
    {
        embed.set_timestamp(std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()));
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
