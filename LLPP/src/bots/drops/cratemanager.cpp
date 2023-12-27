#include "cratemanager.h"
#include "../../common/util.h"
#include "../../core/discord.h"
#include "embeds.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
    namespace
    {
        void set_group_cooldown(std::vector<LootCrateStation>& group)
        {
            for (auto& station : group) {
                station.set_can_default_teleport(false);
                station.set_cooldown();
            }
        }
    }

    CrateManager::CrateManager(const CrateStationParams& t_params,
                               const std::vector<std::vector<QualityFlags>>&
                               t_grouped_crates, suicide::SuicideStation* t_suicide) :
        prefix_(t_params.name), use_beds_(t_params.is_bed_station),
        allowed_render_time_(t_params.max_render_time), align_bed_(prefix_ + "::ALIGN"),
        dropoff_tp_(prefix_ + "::DROPOFF"), dropoff_vault_(prefix_ + "::DROPOFF", 350),
        suicide_(t_suicide)
    {
        populate_groups(t_grouped_crates, t_params.interval);
        register_slash_commands();
    }

    void CrateManager::CrateGroupStatistics::add_looted()
    {
        const auto now = std::chrono::system_clock::now();
        const std::chrono::seconds elapsed = (last_looted != UNDEFINED_TIME)
                                                 ? util::get_elapsed<
                                                     std::chrono::seconds>(last_looted)
                                                 : std::chrono::minutes(30);

        avg_respawn_ = ((avg_respawn_ * times_looted_) + elapsed) / (times_looted_ + 1);
        times_looted_++;
        last_looted = now;
    }

    bool CrateManager::run()
    {
        if (!is_ready_to_run()) { return false; }

        const auto start = std::chrono::system_clock::now();
        if (!use_beds_) { spawn_on_align_bed(); }

        bool any_looted = false;
        run_all_stations(any_looted);
        if (!use_beds_) {
            teleport_to_dropoff();
            if (any_looted) { dropoff_items(last_known_vault_fill_level_); }
        }
        if (suicide_) { suicide_->complete(); }

        send_summary_embed(prefix_, util::get_elapsed<std::chrono::seconds>(start),
                           stats_per_group, last_known_vault_fill_level_ * 100,
                           std::chrono::system_clock::now() + crates_[0][0].
                           get_completion_interval());
        return true;
    }

    bool CrateManager::is_ready_to_run() const { return crates_[0][0].is_ready(); }

    std::chrono::minutes CrateManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            crates_[0][0].get_next_completion());
    }

    core::data::ManagedVar<bool> CrateManager::get_reroll_mode()
    {
        static auto reroll_mode = core::data::ManagedVar<bool>("reroll_mode", false);
        return reroll_mode;
    }

    void CrateManager::run_all_stations(bool& any_looted)
    {
        any_looted = false;
        bool can_default_tp = true;
        bool let_teleporters_render = false;

        int i = 0;
        for (auto& group : crates_) {
            for (auto& station : group) {
                station.set_can_default_teleport(can_default_tp);
                station.set_fully_loot(!get_reroll_mode().get());
                auto result = station.complete();

                if (!let_teleporters_render) {
                    asa::core::sleep_for(std::chrono::seconds(3));
                    let_teleporters_render = true;
                }

                if (result.get_success()) {
                    any_looted = true;
                    stats_per_group[i].add_looted();
                    set_group_cooldown(group);
                    can_default_tp = (&station == &group.back());
                    break;
                }
                can_default_tp = true;
            }
            i++;
        }
    }

    void CrateManager::dropoff_items(float& fill_level_out)
    {
        asa::entities::local_player->turn_up(50, std::chrono::milliseconds(500));
        asa::entities::local_player->turn_right(90);

        asa::entities::local_player->access(dropoff_vault_);
        asa::entities::local_player->get_inventory()->transfer_all();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        fill_level_out = dropoff_vault_.get_slot_count() / dropoff_vault_.get_max_slots();

        dropoff_vault_.inventory->close();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void CrateManager::teleport_to_dropoff()
    {
        asa::entities::local_player->teleport_to(dropoff_tp_);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        asa::entities::local_player->stand_up();
    }

    void CrateManager::spawn_on_align_bed()
    {
        asa::entities::local_player->fast_travel_to(align_bed_);
        asa::entities::local_player->crouch();
        asa::entities::local_player->turn_down(20);
    }

    void CrateManager::populate_groups(
        const std::vector<std::vector<QualityFlags>>& groups,
        std::chrono::minutes interval)
    {
        crates_.resize(groups.size());
        stats_per_group.resize(groups.size());
        int n = 0;

        for (size_t i = 0; i < groups.size(); i++) {
            for (QualityFlags qualities : groups[i]) {
                std::string name = util::add_num_to_prefix(prefix_ + "::DROP", ++n);
                crates_[i].emplace_back(name, qualities, interval, use_beds_,
                                        allowed_render_time_);
            }
        }
    }

    void CrateManager::register_slash_commands()
    {
        dpp::slashcommand crate_commands("crates", "Controls all crate managers.", 0);

        if (!has_registered_reroll_command_) {
            dpp::command_option reroll_field(dpp::co_sub_command, "reroll",
                                             "Manage reroll mode test");

            reroll_field.add_option(dpp::command_option(dpp::co_boolean,
                                                        "toggle",
                                                        "Whether to use reroll mode.",
                                                        true));
            crate_commands.add_option(reroll_field);

            has_registered_reroll_command_ = true;

            core::discord::register_slash_command(crate_commands, reroll_mode_callback);
        }
    }

    void CrateManager::reroll_mode_callback(const dpp::slashcommand_t& event)
    {
        auto cmd_data = event.command.get_command_interaction();

        auto& subcommand = cmd_data.options[0];
        bool enable = subcommand.get_value<bool>(0);
        std::string as_string = enable ? "true" : "false";

        std::cout << "[+] reroll mode changed via discord from" << get_reroll_mode().get()
            << " to " << as_string << std::endl;
        if (get_reroll_mode().get() == enable) {
            return event.reply(std::format("`reroll mode` is already `{}`", as_string));
        }
        get_reroll_mode().set(enable);
        event.reply(std::format("`reroll mode` has been changed to `{}`", as_string));
    }
}
