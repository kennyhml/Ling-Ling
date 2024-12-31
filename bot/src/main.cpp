#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "asa/ui/console.h"
#include <opencv2/core/utils/logger.hpp>
#include <dpp/dpp.h>

#include "asa/core/logging.h"
#include "configuration/validate.h"
#include "core/startup.h"

int main()
{
    setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    asa::setup_logger();

    lingling::startup();
    Sleep(100000);

    return EXIT_SUCCESS;
}
