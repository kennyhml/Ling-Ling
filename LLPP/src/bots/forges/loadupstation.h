#pragma once
#include <asapp/structures/dedicatedstorage.h>
#include "../../core/teleportstation.h"

namespace llpp::bots::forges
{
    class LoadupStation final : public core::TeleportStation
    {
    public:
        explicit LoadupStation(const std::string& t_name, std::string t_material);

        core::StationResult complete() override;

        [[nodiscard]] const std::string& get_material() const { return material_; }

        /**
         * @brief Checks whether this loadup station was empty in the last 15min.
         */
        [[nodiscard]] bool was_recently_empty() const
        {
            return !util::timedout(last_empty_, 15min);
        };

    private:
        bool get_slotcap();

        std::string material_;
        std::array<asa::structures::DedicatedStorage, 6> dedis_;

        std::chrono::system_clock::time_point last_empty_;
    };
}
