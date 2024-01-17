#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/craftingstation.h>
#include <asapp/structures/simplebed.h>
#include <asapp/structures/dedicatedstorage.h>


namespace llpp::bots::paste
{
    class GrindStation final : public core::BaseStation
    {
    public:
        explicit GrindStation(std::string name, std::chrono::minutes interval);

        core::StationResult complete() override;

    private:
        enum State
        {
            WAITING,
            GRINDING,
        };

        std::chrono::system_clock::time_point flint_grind_start_;
        State state_ = GRINDING;

        asa::structures::SimpleBed bed_;
        asa::structures::CraftingStation grinder_;
        asa::structures::DedicatedStorage paste_dedi_;

        std::array<asa::structures::DedicatedStorage, 6> depo_dedis_{
            asa::structures::DedicatedStorage("FLINT01"),
            asa::structures::DedicatedStorage("FLINT02"),
            asa::structures::DedicatedStorage("FLINT03"),
            asa::structures::DedicatedStorage("FLINT04"),
            asa::structures::DedicatedStorage("STONE01"),
            asa::structures::DedicatedStorage("STONE02")
        };

        void take_paste();
        void put_paste_and_grind();
        void empty_grinder();
        void deposit_items();

        [[nodiscard]] bool flint_finished() const;
    };
}
