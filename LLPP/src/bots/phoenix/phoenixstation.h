#pragma once
#include "../../core/bedstation.h"
#include <asapp/structures/dedicatedstorage.h>

namespace llpp::bots::phoenix
{
    class PhoenixStation final : public core::BedStation
    {

    public:
        PhoenixStation(std::string t_name, std::chrono::minutes t_interval);

        core::StationResult complete() override;

    private:
        asa::structures::Container vault_;
        std::array<asa::structures::DedicatedStorage, 6> dedis_;
    };
}
