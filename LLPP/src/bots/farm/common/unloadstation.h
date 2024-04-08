#pragma once
#include "../../../core/teleportstation.h"

namespace llpp::bots::farm
{
    class UnloadStation final : public core::TeleportStation
    {
    public:
        UnloadStation(const std::string& t_name,
                      std::string t_to_unload,
                      std::shared_ptr<asa::entities::DinoEntity> t_anky);

        /**
         * @brief Completes the station by teleporting to it and dropping the metal.
         *
         * @return The result of the station completion.
         */
        core::StationResult complete() override;

    private:
        std::string to_unload_;
        std::shared_ptr<asa::entities::DinoEntity> anky_;
    };
}
