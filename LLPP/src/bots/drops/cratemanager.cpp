#include "cratemanager.h"
#include "../../common/util.h"
#include "../../core/discord.h"
#include "embeds.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
    CrateManager::CrateManager(std::string t_prefix,
                               const std::vector<std::vector<QualityFlags>>&
                               t_grouped_crates,
                               const std::chrono::minutes t_interval,
                               suicide::SuicideStation* t_suicide)
        : prefix(std::move(t_prefix)),
          align_bed(prefix + "::ALIGN"),
          dropoff_tp(prefix + "::DROPOFF"), dropoff_vault(prefix + "::DROPOFF", 350),
          suicide(t_suicide)
    {
        populate_groups(t_grouped_crates, t_interval);
        register_slash_commands();
    }

    void CrateManager::CrateGroupStatistics::add_looted()
    {
        const auto now = std::chrono::system_clock::now();
        std::chrono::seconds elapsedTime =
            (last_looted != UNDEFINED_TIME)
                ? util::get_elapsed<std::chrono::seconds>(last_looted)
                : std::chrono::minutes(30);

        avg_respawn = ((avg_respawn * times_looted) + elapsedTime) /
            (times_looted + 1);

        times_looted++;
        last_looted = now;
    }

    bool CrateManager::run()
    {
        if (!is_ready_to_run())
        {
            return false;
        }

        auto start = std::chrono::system_clock::now();
        spawn_on_align_bed();

        bool wasAnyLooted = false;
        run_all_stations(wasAnyLooted);

        teleport_to_dropoff();
        if (wasAnyLooted)
        {
            dropoff_items(last_known_vault_fill_level);
        }

        access_bed_above();
        if (suicide)
        {
            suicide->complete();
        }

        send_summary_embed(prefix,
                           util::get_elapsed<std::chrono::seconds>(start),
                           stats_per_group,
                           last_known_vault_fill_level * 100,
                           std::chrono::system_clock::now() +
                           crates[0][0].get_completion_interval());
        return true;
    }

    void CrateManager::set_group_cooldown(std::vector<LootCrateStation>& group)
    {
        for (auto& station : group)
        {
            station.set_can_default_teleport(false);
            station.set_cooldown();
        }
    }

    bool CrateManager::is_ready_to_run() const
    {
        return crates[0][0].is_ready();
    }

    std::chrono::minutes CrateManager::get_time_left_until_ready() const
    {
        return util::get_time_left_until<std::chrono::minutes>(
            crates[0][0].get_next_completion());
    }

    core::data::ManagedVar<bool> CrateManager::get_reroll_mode()
    {
        static auto reroll_mode = core::data::ManagedVar<bool>(
            "reroll_mode", false);
        return reroll_mode;
    }

    void CrateManager::run_all_stations(bool& anyLooted)
    {
        anyLooted = false;
        bool canDefaultTeleport = true;
        bool let_teleporters_render = false;

        int i = 0;
        for (auto& group : crates)
        {
            for (auto& station : group)
            {
                station.set_can_default_teleport(canDefaultTeleport);
                station.set_fully_loot(!get_reroll_mode().get());
                auto result = station.complete();

                if (!let_teleporters_render)
                {
                    asa::core::sleep_for(std::chrono::seconds(3));
                    let_teleporters_render = true;
                }

                if (result.get_success())
                {
                    anyLooted = true;
                    stats_per_group[i].add_looted();
                    set_group_cooldown(group);
                    canDefaultTeleport = (&station == &group.back());
                    break;
                }
                canDefaultTeleport = true;
            }
            i++;
        }
    }

    void CrateManager::dropoff_items(float& filledLevelOut)
    {
        asa::entities::local_player->turn_up(
            50, std::chrono::milliseconds(500));
        asa::entities::local_player->turn_right(90);

        asa::entities::local_player->access(this->dropoff_vault);
        asa::entities::local_player->get_inventory()->transfer_all();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        filledLevelOut = dropoff_vault.get_slot_count() /
            dropoff_vault.get_max_slots();

        this->dropoff_vault.inventory->close();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void CrateManager::teleport_to_dropoff()
    {
        asa::entities::local_player->teleport_to(dropoff_tp);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        asa::entities::local_player->stand_up();
    }

    void CrateManager::access_bed_above()
    {
        asa::entities::local_player->look_fully_up();
        asa::entities::local_player->access(align_bed);
    }

    void CrateManager::spawn_on_align_bed()
    {
        asa::entities::local_player->fast_travel_to(align_bed);
        asa::entities::local_player->crouch();
        asa::entities::local_player->turn_down(20);
    }

    void CrateManager::populate_groups(
        const std::vector<std::vector<QualityFlags>>& groups,
        std::chrono::minutes interval)
    {
        crates.resize(groups.size());
        stats_per_group.resize(groups.size());
        int n = 0;

        for (size_t i = 0; i < groups.size(); i++)
        {
            for (QualityFlags qualities : groups[i])
            {
                std::string name = util::add_num_to_prefix(
                    prefix + "::DROP", ++n);
                crates[i].push_back({name, qualities, interval});
            }
        }
    }

    void CrateManager::register_slash_commands()
    {
        dpp::slashcommand crates("crates", "Controls all crate managers.", 0);

        if (!has_registered_reroll_command)
        {
            dpp::command_option reroll_field(
                dpp::co_sub_command, "reroll", "Manage reroll mode test");

            reroll_field.add_option(dpp::command_option(dpp::co_boolean,
                                                        "toggle",
                                                        "Whether to use reroll mode.",
                                                        true));

            crates.add_option(reroll_field);

            has_registered_reroll_command = true;

            core::discord::register_slash_command(crates, reroll_mode_callback);
        }
    }

    void CrateManager::reroll_mode_callback(const dpp::slashcommand_t& event)
    {
        auto cmd_data = event.command.get_command_interaction();

        auto& subcommand = cmd_data.options[0];
        bool enable = subcommand.get_value<bool>(0);
        std::string as_string = enable ? "true" : "false";

        std::cout << "[+] reroll mode changed via discord from"
            << get_reroll_mode().get() << " to " << as_string
            << std::endl;
        if (get_reroll_mode().get() == enable)
        {
            return event.reply(
                std::format("`reroll mode` is already `{}`", as_string));
        }
        get_reroll_mode().set(enable);
        event.reply(
            std::format("`reroll mode` has been changed to `{}`", as_string));
    }
}
