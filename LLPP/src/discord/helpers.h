#pragma once
#include <string>
#include <dpp/dpp.h>
#include <opencv2/imgproc.hpp>

namespace llpp::discord
{
    /**
     * @brief Gets the string buffer representation of a cv::Mat.
     *
     * @param source The mat to get the string buffer of.
     *
     * @return The string buffer data of the image that dpp wants.
     */
    std::string strbuf(const cv::Mat& source);

    /**
     * @brief Sets the timestamp on the embed as the current time.
     *
     * @param embed The embed to set the timestamp on.
     */
    void set_now_timestamp(dpp::embed& embed);

    std::string get_role_mention(const std::string& role);
    std::string get_user_mention(const std::string& user);

}
