#pragma once
#include "loadupstation.h"
#include "unloadstation.h"
#include "../../core/bedstation.h"
#include "../../core/teleportstation.h"


namespace llpp::bots::forges
{
    class ForgeStation : public core::BedStation
    {
    public:
        ForgeStation(const std::string& t_name,
                     int32_t t_view_diff,
                     std::shared_ptr<std::vector<LoadupStation> > t_loadup_stations,
                     std::shared_ptr<std::vector<UnloadStation> > t_unload_stations);

        core::StationResult complete() override;

        bool is_ready() override
        {
            return BedStation::is_ready() && has_finished_cooking();
        }

        /**
         * @brief Checks whether the station is currently cooking any material.
         */
        [[nodiscard]] bool is_empty() const;

        /**
         * @brief Checks whether the was cooking any material but has finished.
         */
        [[nodiscard]] bool has_finished_cooking() const;

    private:
        inline static core::TeleportStation mediator_station_{"FORGES::MEDIATOR", 0min};

        bool empty_forges();

        void fill_all();

        bool unload(const std::string& material);

        bool loadup(bool is_at_unload, std::string& material_out);

        void empty(const asa::structures::Container& forge);

        void fill(const asa::structures::Container& forge);

        std::string get_last_material() const;

        int64_t get_last_filled() const;

        [[nodiscard]] std::chrono::minutes get_current_material_cook_time() const;

        int32_t view_diff_;
        core::TeleportStation fill_tp_;

        std::shared_ptr<std::vector<LoadupStation> > loadup_stations_;
        std::shared_ptr<std::vector<UnloadStation> > unload_stations_;

        std::array<asa::structures::Container, 3> forges_;
    };
}
