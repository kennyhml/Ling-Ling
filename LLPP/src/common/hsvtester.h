#pragma once
#include <opencv2/core/mat.hpp>

#include "hsvfilter.h"

namespace llpp::util
{
    class HsvTester
    {
    public:
        HsvTester(cv::Mat source, cv::Mat templ);
        ~HsvTester();

        void run();

        [[nodiscard]] const HsvFilter& get_current_filter() const { return filter_; }

        void static filter_changed(int pos, void* data);

    private:
        inline static int num_instances_ = 0;

        HsvFilter filter_;


        void create_windows();

        std::string filter_win_;
        std::string source_win_;
        std::string template_win_;

        cv::Mat original_source_;
        cv::Mat original_template_;

        cv::Mat current_source_;
        cv::Mat current_template_;
    };
}
