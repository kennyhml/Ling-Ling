#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <opencv2/core/utils/logger.hpp>
#include <dpp/dpp.h>

#include "asa/core/logging.h"
#include "asa/core/managedthread.h"
#include "asa/core/state.h"
#include "asa/game/window.h"
#include "asa/items/items.h"
#include "core/startup.h"
#include "core/state.h"
#include "modules/gacha/stations/seedstation.h"
#include "tasksystem/persistenttask.h"
#include "tasksystem/queue.h"


struct test_task : lingling::persistent_task
{
    using persistent_task::persistent_task;

    lingling::task_result& execute() override
    {
        last_started_ = std::chrono::system_clock::now();
        last_completion_ = std::chrono::system_clock::now();
        asa::get_logger()->info("Test class {} executed.", id_);
        asa::checked_sleep(20s);
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


    asa::checked_sleep(3s);
    asa::get_window_handle();
    asa::load_items();
    asa::load_game_settings("F:/SteamLibrary/steamapps/common/ARK Survival Ascended");

    asa::set_window_focus();

    auto station = lingling::gacha::seedstation("test", std::make_shared<asa::teleporter>("test"));
    station.set_seeding_flags(lingling::gacha::SEEDING_RESTOCK_FOR_NEXT);
    station.complete();

    asa::checked_sleep(10s);

    station.set_seeding_flags(lingling::gacha::SEEDING_NO_BERRY_TRANSFER);
    station.complete();
    return 1;

    lingling::startup();




    // Fake task provider for testing purposes, alwasy provides a task
    lingling::add_task_queue_task_provider(
        []() -> std::vector<std::shared_ptr<lingling::task> > {
            auto tk = std::make_shared<test_task>("gacha", "gachaseed01", "Text",
                                             lingling::task_priority::PRIORITY_HIGH);
            tk->set_state(lingling::task_state::STATE_QUEUEABLE);
            asa::get_logger()->info("CALLED");
            return {tk};
        });

    while (true) { lingling::check_for_user_keyboard_input_action(); }
    return EXIT_SUCCESS;
}
