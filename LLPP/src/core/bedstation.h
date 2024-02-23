#pragma once
#include <asapp/structures/simplebed.h>

#include "basestation.h"

namespace llpp::core
{
    class BedStation : public BaseStation
    {
    public:
        BedStation(std::string t_name, std::chrono::minutes t_interval);
        BedStation(std::string t_name,
                   std::chrono::system_clock::time_point t_last_completed,
                   std::chrono::minutes t_interval);

        StationResult complete() override;

    protected:
        /**
         * @brief Spawns at the bed, checks logs and sets the start time.
         */
        [[nodiscard]] bool begin(bool check_logs = true) override;

        asa::structures::SimpleBed spawn_bed_;
    };
}
