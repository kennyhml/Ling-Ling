#pragma once
#include <asapp/entities/localplayer.h>
#include <asapp/structures/teleporter.h>
#include "basestation.h"

namespace llpp::core
{
    class TeleportStation : public BaseStation
    {
    public:
        TeleportStation(std::string t_name, std::chrono::minutes t_interval);
        TeleportStation(std::string t_name,
                   std::chrono::system_clock::time_point t_last_completed,
                   std::chrono::minutes t_interval);


        StationResult complete() override;

        /**
         * @brief Sets this station as the default destination of the current teleporter.
         *
         * @param is_default Whether this station is the current default destination.
         */
        void set_tp_is_default(const bool is_default) { is_default_ = is_default; }

    protected:
        /**
         * @brief Starts the station by tping there, does not check logs.
         */
        [[nodiscard]] bool begin(bool check_logs) override;

        [[nodiscard]] TeleportFlags get_teleport_flags() const;

        bool is_default_ = false;
        bool teleport_unsafe_ = false;

        asa::structures::Teleporter start_tp_;
    };
}
