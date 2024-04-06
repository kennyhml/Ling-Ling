#pragma once

#include "../../core/istationmanager.h"
#include "sparkpowderstation.h"
#include "gunpowderstation.h"
#include "grindstation.h"
#include "arbstation.h"
#include <dpp/dpp.h>

namespace llpp::bots::crafting
{
    class CraftingManager final : public core::IStationManager
    {
    public:
        CraftingManager();

        ~CraftingManager() override;

        bool run() override;

        [[nodiscard]] bool is_ready_to_run() override;

        [[nodiscard]] std::chrono::minutes get_time_left_until_ready() const override;

    private:
        void register_slash_commands();

        static void slashcommand_callback(const dpp::slashcommand_t& event);

        void run_spark() const;

        void run_gunpowder() const;

        void run_grinding() const;

        void run_arb() const;

        [[nodiscard]] bool is_spark_ready() const;

        [[nodiscard]] bool is_gunpowder_ready() const;

        [[nodiscard]] bool is_grinding_ready() const;

        [[nodiscard]] bool is_arb_ready() const;

        bool has_registered_commands_ = false;

        core::BaseStation* find_station(const std::string& name) const;

        std::vector<std::unique_ptr<SparkpowderStation>> spark_stations_;
        std::vector<std::unique_ptr<GunpowderStation>> gunpowder_stations_;
        std::vector<std::unique_ptr<GrindStation>> grinding_stations_;
        std::vector<std::unique_ptr<ARBStation>> arb_stations_;


    };
}
