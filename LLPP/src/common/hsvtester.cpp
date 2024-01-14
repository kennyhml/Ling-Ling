#include "hsvtester.h"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <asapp/game/window.h>

namespace llpp::util
{
    namespace
    {
        void shift_channel(const cv::Mat& channel, const int amount)
        {
            if (amount > 0) {
                int lim = 255 - amount;
                threshold(channel, channel, lim, 255, cv::THRESH_TOZERO);
                channel += amount;
            }
            else if (amount < 0) {
                int lim = -amount;
                threshold(channel, channel, lim, 255, cv::THRESH_TOZERO_INV);
                channel -= amount;
            }
        }

        void apply_filter(const HsvFilter& filter, const cv::Mat& src, cv::Mat& dst)
        {
            cvtColor(src, dst, cv::COLOR_RGB2HSV);

            std::vector<cv::Mat> channels;
            split(dst, channels);

            shift_channel(channels[1], filter.sat_add);
            shift_channel(channels[1], -filter.sat_sub);

            shift_channel(channels[2], -filter.val_sub);
            shift_channel(channels[2], filter.val_add);
            cv::merge(channels, dst);

            const auto high = cv::Scalar(filter.hue_min, filter.sat_min, filter.val_min);
            const auto low = cv::Scalar(filter.hue_max, filter.sat_max, filter.val_max);

            cv::Mat mask;
            inRange(dst, high, low, mask);

            cv::Mat result;
            bitwise_and(dst, dst, result, mask);
            cvtColor(result, dst, cv::COLOR_HSV2RGB);
        }
    }
    
    HsvTester::HsvTester(cv::Mat source, cv::Mat templ) : original_source_(source),
                                                          original_template_(templ)
    {
        num_instances_++;
        create_windows();
    }

    HsvTester::~HsvTester()
    {
        cv::destroyWindow(source_win_);
        cv::destroyWindow(template_win_);
        cv::destroyWindow(filter_win_);
        num_instances_--;
    }

    void HsvTester::filter_changed(int pos, void* data)
    {
        auto inst = static_cast<HsvTester*>(data);
        inst->original_source_ = asa::window::screenshot({1880, 954, 11, 45});
        apply_filter(inst->filter_, inst->original_source_, inst->current_source_);
        apply_filter(inst->filter_, inst->original_template_, inst->current_template_);

        imshow(inst->source_win_, inst->current_source_);
        // imshow(inst->template_win_, inst->current_template_);
    }

    void HsvTester::create_windows()
    {
        source_win_ = "HSV-Test Source " + std::to_string(num_instances_);
        template_win_ = "HSV-Test Template " + std::to_string(num_instances_);
        filter_win_ = "HSV-Test Filter " + std::to_string(num_instances_);

        namedWindow(source_win_, cv::WINDOW_AUTOSIZE);
        namedWindow(template_win_, cv::WINDOW_AUTOSIZE);
        namedWindow(filter_win_, cv::WINDOW_AUTOSIZE);

        const auto& win = filter_win_;

        cv::createTrackbar("Min Hue", win, &filter_.hue_min, 179, filter_changed, this);
        cv::createTrackbar("Max Hue", win, &filter_.hue_max, 179, filter_changed, this);

        cv::createTrackbar("Min Sat", win, &filter_.sat_min, 255, filter_changed, this);
        cv::createTrackbar("Max Sat", win, &filter_.sat_max, 255, filter_changed, this);
        cv::createTrackbar("Sat add", win, &filter_.sat_add, 255, filter_changed, this);
        cv::createTrackbar("Sat sub", win, &filter_.sat_sub, 255, filter_changed, this);

        cv::createTrackbar("Min Val", win, &filter_.val_min, 255, filter_changed, this);
        cv::createTrackbar("Max Val", win, &filter_.val_max, 255, filter_changed, this);
        cv::createTrackbar("Val add", win, &filter_.val_add, 255, filter_changed, this);
        cv::createTrackbar("Val sub", win, &filter_.val_sub, 255, filter_changed, this);
    }
}
