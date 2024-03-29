#pragma once
#include "../../core/teleportstation.h"

namespace llpp::bots::metal
{
    class UnloadStation final : public core::TeleportStation
    {
    public:
        UnloadStation(const std::string& t_name,
                      std::shared_ptr<asa::entities::DinoEntity> t_anky);

        /**
         * @brief Completes the station by teleporting to it and dropping the metal.
         *
         * @return The result of the station completion.
         */
        core::StationResult complete() override;

    private:
        std::shared_ptr<asa::entities::DinoEntity> anky_;
    };
}
