#pragma once
#include <chrono>

namespace llpp::bots::boss
{
    struct DepositResult final
    {
    public:
        bool succeeded;
        bool deposited_into_dedi;

        std::chrono::seconds time_taken;
    };

    /**
     * @brief Deposits the element
     */
    DepositResult deposit_element();
}
