#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "asa/ui/console.h"
#include <opencv2/core/utils/logger.hpp>
#include <dpp/dpp.h>

#include "asa/core/logging.h"
#include "core/startup.h"
#include "tasksystem/discord.h"
#include "tasksystem/persistenttask.h"
#include "tasksystem/queue.h"
#include "integration/discord/commands/tribelogrole.h"

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
    lingling::discord::listener();
    lingling::startup();


    Sleep(100000);

    return EXIT_SUCCESS;
}
