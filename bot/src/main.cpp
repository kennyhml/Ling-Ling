#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "asa/ui/console.h"
#include <opencv2/core/utils/logger.hpp>
#include <dpp/dpp.h>

#include "asa/core/logging.h"
#include "core/startup.h"
#include "tasksystem/persistenttask.h"
#include "tasksystem/queue.h"

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

    lingling::startup();

    auto t1 = std::make_shared<test>("Gacha Tower", "Gacha33-34", "Feed Gacha (Seeds).", lingling::task_priority::PRIORITY_LOW);
    auto t2 = std::make_shared<test>("Gacha Tower", "Gacha01-02", "Feed Gacha (Seeds).", lingling::task_priority::PRIORITY_HIGH);
    auto t3 = std::make_shared<test>("Gacha Tower", "Gacha10-11", "Feed Gacha (Seeds).", lingling::task_priority::PRIORITY_HIGH);
    auto t4 = std::make_shared<test>("Gacha Tower", "Gacha40-41", "Feed Gacha (Seeds).", lingling::task_priority::PRIORITY_MEDIUM);
    auto t5 = std::make_shared<test>("Gacha Tower", "Gacha42-43", "Feed Gacha (Seeds).", lingling::task_priority::PRIORITY_MEDIUM);
    auto t6 = std::make_shared<test>("Gacha Tower", "Gacha44-45", "Feed Gacha (Seeds).", lingling::task_priority::PRIORITY_LOW);
    auto t7 = std::make_shared<test>("Gacha Tower", "Gacha44-45", "Wake up sleepy and wish him a happy new year.", lingling::task_priority::PRIORITY_CRITICAL);
    auto t8 = std::make_shared<test>("Gacha Tower", "Gacha44-45", "Wake up sleepy and wish him a happy new year.", lingling::task_priority::PRIORITY_CRITICAL);
    auto t9 = std::make_shared<test>("Gacha Tower", "Gacha44-45", "Wake up sleepy and wish him a happy new year.", lingling::task_priority::PRIORITY_CRITICAL);
    auto t10 = std::make_shared<test>("Gacha Tower", "Gacha44-45", "Wake up sleepy and wish him a happy new year.", lingling::task_priority::PRIORITY_CRITICAL);


    t1->set_state(lingling::task_state::STATE_WAITING);
    t2->set_state(lingling::task_state::STATE_WAITING);
    t3->set_state(lingling::task_state::STATE_WAITING);
    t4->set_state(lingling::task_state::STATE_WAITING);
    t5->set_state(lingling::task_state::STATE_WAITING);
    t6->set_state(lingling::task_state::STATE_WAITING);
    t7->set_state(lingling::task_state::STATE_WAITING);
    t8->set_state(lingling::task_state::STATE_WAITING);
    t9->set_state(lingling::task_state::STATE_WAITING);
    t10->set_state(lingling::task_state::STATE_WAITING);
    lingling::enqueue_task(t1);
    lingling::enqueue_task(t2);
    lingling::enqueue_task(t3);
    lingling::enqueue_task(t4);
    lingling::enqueue_task(t5);
    lingling::enqueue_task(t6);
    lingling::enqueue_task(t7);
    lingling::enqueue_task(t8);
    lingling::enqueue_task(t9);
    lingling::enqueue_task(t10);
    lingling::get_taskqueue_view();

    lingling::add_task_enqueue_lookup_listener([&t1] () -> std::shared_ptr<test> {
        if (t1->get_state() != lingling::task_state::STATE_ENQUEUED) {
            return t1;
        }
        return nullptr;
    });
    lingling::start_queue_handler_thread();

    Sleep(100000);

    return EXIT_SUCCESS;
}
