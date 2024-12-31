#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "asa/ui/console.h"
#include <opencv2/core/utils/logger.hpp>
#include <dpp/dpp.h>

#include "asa/core/logging.h"
#include "configuration/validate.h"
#include "core/startup.h"
#include "tasksystem/persistenttask.h"

struct test : lingling::persistent_task
{
    using persistent_task::persistent_task;

    lingling::task_result& execute() override
    {
        last_completion_ = std::chrono::system_clock::now();
        return last_result_;
    }

    bool is_ready() override
    {
        return false;
    }
};


int main()
{
    setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
    asa::setup_logger();

    test t("test", "myid", "This is a description", lingling::task_priority::PRIORITY_MEDIUM, 30min);

    t.execute();
    t.dump_data();

    return 1;

    lingling::startup();
    Sleep(100000);

    return EXIT_SUCCESS;
}
