#pragma once

#include "cratestation.h"


namespace llpp::bots::drops
{
    class BedCrateStation final : public core::BedStation, public CrateStation
    {
    public:

        BedCrateStation(const std::string& t_name, CrateManagerConfig* t_config,
                        asa::structures::CaveLootCrate t_crate, bool t_is_first,
                        bool t_is_first_in_group);


        core::StationResult complete() override;

        const asa::structures::Container& get_vault() const { return vault_; }

    private:

        void deposit_items();

        asa::structures::Container vault_;
    };
}
