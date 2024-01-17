#pragma once
#include <chrono>
#include <string>
#include <asapp/structures/container.h>
#include <asapp/structures/simplebed.h>
#include <asapp/structures/craftingstation.h>
#include "../../core/basestation.h"

namespace llpp::bots::crafting
{
    class GunpowderStation : public core::BaseStation
    {
    public:
        GunpowderStation(const std::string& t_name, std::chrono::minutes t_interval);

        core::StationResult complete() override;

        bool is_ready() const override { return !disabled_ && BaseStation::is_ready(); }


        [[nodiscard]] bool get_disabled() const { return disabled_; }
        void set_disabled(const bool disabled) { disabled_ = disabled; }

    private:
        enum State
        {
            WAITING,
            CRAFTING,
        };

        [[nodiscard]] bool has_crafting_finished() const;

        void empty_chembench();
        void queue_new();

        std::chrono::system_clock::time_point crafting_start_;

        State state_ = CRAFTING;
        bool disabled_ = false;

        asa::structures::CraftingStation chem_bench_;
        asa::structures::SimpleBed bed_;

        std::array<asa::structures::Container, 3> dedis_{
            asa::structures::Container("DEDI01", 1800),
            asa::structures::Container("DEDI02", 1800),
            asa::structures::Container("DEDI03", 1800),
        };
    };
}
