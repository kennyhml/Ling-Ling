#include "cratemanager.h"
#include "embeds.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../discord/bot.h"
#include <asapp/core/state.h>
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
    namespace
    {
        void set_group_cooldown(std::vector<BedCrateStation>& group)
        {
            for (auto& station : group) { station.suspend_for(std::chrono::minutes(1)); }
        }

        void set_group_cooldown(std::vector<TeleportCrateStation>& group)
        {
            for (auto& station : group) {
                station.set_default_destination(false);
                station.suspend_for(std::chrono::minutes(1));
            }
        }

        template <typename T>
        void set_group_rendered(std::vector<T>& group, const bool rendered)
        {
            for (auto& station : group) { station.set_rendered(rendered); }
        }


        template <typename T>
        bool all_ready(std::vector<std::vector<T>>& stations)
        {
            if (stations.empty()) { return false; }

            for (auto& group : stations) {
                for (auto& station : group) {
                    if (!station.is_ready()) { return false; }
                }
            }
            return true;
        }

        template <typename T>
        std::vector<std::vector<T>> parse_stations(CrateManagerConfig* config)
        {
            std::vector<std::vector<T>> ret;
            std::vector<int> lefts;
            std::vector<int> rights;

            std::string str = config->grouped_crates_raw;

            std::erase_if(str, isspace);

            // keep track of the positions of the left and right curly brackets
            for (int i = 0; i < str.size(); i++) {
                if (str.at(i) == '{') { lefts.push_back(i); }
                else if (str.at(i) == '}') { rights.push_back(i); }
            }

            if (lefts.size() != rights.size()) {
                throw std::exception("Left brackets do not match right brackets");
            }
            ret.resize(lefts.size());

            int group = 0;
            int num_station = 1;
            for (int left : lefts) {
                std::string roi = str.substr(left + 1, rights[group] - left - 1);
                std::vector<int> commas;
                for (int j = 0; j < roi.size(); j++) {
                    if (roi.at(j) == ',') { commas.push_back(j); }
                }

                for (int crate = 0; crate < commas.size() + 1; crate++) {
                    int prev = crate > 0 ? commas[crate - 1] + 1 : 0;
                    int end = commas.empty() || crate >= commas.size()
                                  ? roi.size()
                                  : commas[crate] - prev;

                    QualityFlags flag = 0;
                    std::string color = roi.substr(prev, end);
                    std::vector<std::string> tokens;
                    std::istringstream stream(color);
                    std::string token;

                    while (std::getline(stream, token, '|')) {
                        flag |= asa::structures::CaveLootCrate::string_to_quality(token);
                    }
                    const bool first = num_station == 1;
                    const bool first_in_grp = crate == 0;
                    ret[group].emplace_back(
                        util::add_num_to_prefix(config->prefix + "::DROP", num_station++),
                        config, asa::structures::CaveLootCrate(flag), first,
                        first_in_grp);
                }
                group++;
            }
            return ret;
        }
    }

    CrateManager::CrateManager(CrateManagerConfig* t_config) : config_(t_config),
        teleport_align_station_(config_->prefix + "::ALIGN", std::chrono::minutes(5)),
        beds_out_station_(config_->prefix + "::DROPS::OUT", std::chrono::minutes(5)),
        teleport_dropoff_station_(config_->prefix + "::DROPOFF", std::chrono::minutes(0)),
        dropoff_vault_(config_->prefix + "::DROPOFF", 350)
    {
        if (config_->uses_teleporters) {
            teleport_stations_ = parse_stations<TeleportCrateStation>(config_);
            stats_per_group.resize(teleport_stations_.size());
        }
        else {
            bed_stations_ = parse_stations<BedCrateStation>(config_);
            stats_per_group.resize(bed_stations_.size());
        }
        register_slash_commands();
    }

    void CrateManager::CrateGroupStatistics::add_looted()
    {
        const auto now = std::chrono::system_clock::now();
        auto elapsed = util::get_elapsed<std::chrono::seconds>(last_looted);
        if (elapsed > std::chrono::hours(2)) { elapsed = std::chrono::minutes(30); }

        avg_respawn_ = ((avg_respawn_ * times_looted_) + elapsed) / (times_looted_ + 1);
        times_looted_++;
        last_looted = now;
    }

    bool CrateManager::run()
    {
        if (!is_ready_to_run()) { return false; }

        const auto start = std::chrono::system_clock::now();

        if (config_->uses_teleporters) { run_teleport_stations(); }
        else { run_bed_stations(); }

        const auto time_taken = util::get_elapsed<std::chrono::seconds>(start);
        const auto next = std::chrono::system_clock::now() + get_time_left_until_ready();
        const auto msg = get_summary_message(config_->prefix, time_taken, stats_per_group,
                                             vaults_filled_, next);

        discord::get_bot()->message_create(msg);
        return true;
    }

    void CrateManager::run_bed_stations()
    {
        for (int i = 0; i < bed_stations_.size(); i++) {
            set_group_rendered(bed_stations_[i], false);
            for (auto& station : bed_stations_[i]) {
                if (!station.is_ready()) { continue; }

                const auto result = station.complete();
                set_group_rendered(bed_stations_[i], true);

                if (!result.success) { continue; }

                const int slots = station.get_vault().get_last_known_slots();
                vaults_filled_[station.get_name()] = static_cast<float>(slots) / 350.f;
                stats_per_group[i].add_looted();
                set_group_cooldown(bed_stations_[i]);
                break;
            }
        }

        beds_out_station_.complete();
    }


    void CrateManager::run_teleport_stations()
    {
        std::vector<LootResult> loot;
        bool can_default_tp = true;

        spawn_on_align_bed();

        for (int i = 0; i < teleport_stations_.size(); i++) {
            set_group_rendered(teleport_stations_[i], false);
            for (auto& station : teleport_stations_[i]) {
                if (!station.is_ready()) {
                    if (i == 0 && &station == &teleport_stations_[i].front()) {
                        // Here we are in trouble, the first station currently
                        // shouldnt be used for whatever reason that may be,
                        // but its the default target, so this run wont work out.
                        return;
                    }
                    can_default_tp = false;
                    continue;
                }
                station.set_default_destination(can_default_tp);
                const auto result = station.complete();
                set_group_rendered(teleport_stations_[i], true);

                if (result.success) {
                    const auto& new_loot = station.get_previous_loot();
                    loot.insert(loot.end(), new_loot.begin(), new_loot.end());
                    stats_per_group[i].add_looted();
                    set_group_cooldown(teleport_stations_[i]);
                    can_default_tp = (&station == &teleport_stations_[i].back());
                    break;
                }
                can_default_tp = true;
            }
        }
        dropoff_.set_to_sort(loot);
        dropoff_.complete();
    }

    bool CrateManager::is_ready_to_run()
    {
        if (config_->disabled) { return false; }

        if (config_->uses_teleporters) {
            return teleport_align_station_.is_ready() && all_ready(teleport_stations_);
        }
        return beds_out_station_.is_ready() && all_ready(bed_stations_);
    }

    std::chrono::minutes CrateManager::get_time_left_until_ready() const
    {
        const auto next = config_->uses_teleporters
                              ? teleport_stations_[0][0].get_next_completion()
                              : bed_stations_[0][0].get_next_completion();
        return util::get_time_left_until<std::chrono::minutes>(next);
    }

    void CrateManager::spawn_on_align_bed()
    {
        auto result = teleport_align_station_.complete();
        asa::entities::local_player->crouch();
        asa::entities::local_player->turn_down(20);

        asa::core::sleep_for(std::chrono::seconds(config_->render_align_for));
    }

    void CrateManager::register_slash_commands()
    {
        dpp::slashcommand crate_commands("crates", "Controls all crate managers.", 0);

        // reroll command must only be registered once for the first crate
        // manager to be crated.
        if (!has_registered_reroll_command_) {
            dpp::command_option reroll_group(dpp::co_sub_command_group, "reroll",
                                             "Manage reroll mode test");

            const auto set(dpp::command_option(dpp::co_boolean, "toggle",
                                               "Whether to use reroll mode.", true));

            reroll_group.add_option(dpp::command_option(
                dpp::co_sub_command, "set", "Set reroll mode").add_option(set));

            reroll_group.add_option(dpp::command_option(
                dpp::co_sub_command, "get", "Get current reroll mode"));
            crate_commands.add_option(reroll_group);

            has_registered_reroll_command_ = true;
            discord::register_slash_command(crate_commands, reroll_mode_callback);
        }
    }

    void CrateManager::reroll_mode_callback(const dpp::slashcommand_t& event)
    {
        if (discord::handle_unauthorized_command(event)) { return; }

        auto cmd_data = event.command.get_command_interaction();

        auto& subcommand = cmd_data.options[0].options[0];

        if (subcommand.name == "get") {
            return event.reply(std::format("Reroll mode is currently off"));
        }

        const bool enable = subcommand.get_value<bool>(0);
        std::string as_string = enable ? "true" : "false";

        auto& reroll = config::bots::drops::reroll_mode;
        if (reroll.get() == enable) {
            return event.reply(std::format("`reroll mode` is already `{}`", as_string));
        }
        reroll.set(enable);
        event.reply(std::format("`reroll mode` has been changed to `{}`", as_string));
    }
}
