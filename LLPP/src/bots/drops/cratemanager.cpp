#include "cratemanager.h"
#include "../../common/util.h"
#include "../../config/config.h"
#include "../../core/discord.h"
#include "embeds.h"
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
	namespace
	{
		void set_group_cooldown(std::vector<LootCrateStation>& group)
		{
			for (auto& station : group) {
				station.set_default_dst(false);
				station.set_cooldown();
			}
		}

		void set_group_rendered(
			std::vector<LootCrateStation>& group, const bool rendered)
		{
			for (auto& station : group) {
				station.set_rendered(rendered);
			}
		}
	}

	CrateManager::CrateManager(CrateManagerConfig& t_config)
		: config_(t_config), align_bed_(config_.prefix + "::ALIGN"),
		  dropoff_tp_(config_.prefix + "::DROPOFF"),
		  dropoff_vault_(config_.prefix + "::DROPOFF", 350)
	{
		parse_groups(config_.grouped_crates_raw);
		register_slash_commands();
	}

	void CrateManager::CrateGroupStatistics::add_looted()
	{
		const auto now = std::chrono::system_clock::now();
		const std::chrono::seconds elapsed =
			(last_looted != UNDEFINED_TIME)
				? util::get_elapsed<std::chrono::seconds>(last_looted)
				: std::chrono::minutes(30);

		avg_respawn_ = ((avg_respawn_ * times_looted_) + elapsed) /
					   (times_looted_ + 1);
		times_looted_++;
		last_looted = now;
	}

	bool CrateManager::run()
	{
		if (!is_ready_to_run()) {
			return false;
		}

		const auto start = std::chrono::system_clock::now();
		if (config_.uses_teleporters) {
			spawn_on_align_bed();
		}

		bool any_looted = false;
		run_all_stations(any_looted);
		if (config_.uses_teleporters) {
			teleport_to_dropoff();
			if (any_looted) {
				dropoff_items();
			}
		}

		send_summary_embed(config_.prefix,
			util::get_elapsed<std::chrono::seconds>(start), stats_per_group,
			vault_fill_levels_,
			std::chrono::system_clock::now() +
				crates_[0][0].get_completion_interval());
		return true;
	}

	bool CrateManager::is_ready_to_run() const
	{
		return !is_disabled() && crates_[0][0].is_ready();
	}

	std::chrono::minutes CrateManager::get_time_left_until_ready() const
	{
		return util::get_time_left_until<std::chrono::minutes>(
			crates_[0][0].get_next_completion());
	}

	void CrateManager::run_all_stations(bool& any_looted)
	{
		any_looted = false;
		bool can_default_tp = true;

		int i = 0;
		for (auto& group : crates_) {
			set_group_rendered(group, false);
			for (auto& station : group) {
				station.set_default_dst(can_default_tp);
				const auto result = station.complete();
				set_group_rendered(group, true);

				if (result.get_success()) {
					any_looted = true;
					if (!config_.uses_teleporters) {
						vault_fill_levels_[station.get_name()] =
							static_cast<float>(station.get_vault_slots()) /
							350.f;
					}
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

	void CrateManager::dropoff_items()
	{
		asa::entities::local_player->turn_up(
			50, std::chrono::milliseconds(500));
		asa::entities::local_player->turn_right(90);

		asa::entities::local_player->access(dropoff_vault_);
		asa::entities::local_player->get_inventory()->transfer_all();
		std::this_thread::sleep_for(std::chrono::seconds(3));
		vault_fill_levels_[dropoff_vault_.name] =
			static_cast<float>(dropoff_vault_.get_slot_count()) / 350.f;

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

	void CrateManager::parse_groups(std::string groups)
	{
		std::erase_if(groups, isspace);

		std::vector<int> lefts;
		std::vector<int> rights;

		for (int i = 0; i < groups.size(); i++) {
			if (groups.at(i) == '{') {
				lefts.push_back(i);
			}
			else if (groups.at(i) == '}') {
				rights.push_back(i);
			}
		}

		if (lefts.size() != rights.size()) {
			throw std::exception(
				"Num left brackets does not match num right brackets");
		}

		std::cout << "[+] Allocating for " << lefts.size() << " groups...\n";
		crates_.resize(lefts.size());
		stats_per_group.resize(lefts.size());

		int group = 0;
		int num_station = 1;
		for (int left : lefts) {
			std::string roi = groups.substr(left + 1, rights[group] - left - 1);
			std::vector<int> commas;
			for (int j = 0; j < roi.size(); j++) {
				if (roi.at(j) == ',') {
					commas.push_back(j);
				}
			}

			std::cout << "Parsing " << roi << ", expecting "
					  << commas.size() + 1 << " crates\n";

			for (int crate = 0; crate < commas.size() + 1; crate++) {
				int prev = crate > 0 ? commas[crate - 1] + 1 : 0;
				int end = commas.empty() || crate >= commas.size()
							  ? roi.size()
							  : commas[crate] - prev;

				QualityFlags curr_flag = 0;
				std::string color = roi.substr(prev, end);
				std::cout << "Parsing color of '" << color << "'\n";

				std::vector<std::string> tokens;
				std::istringstream stream(color);
				std::string token;

				while (std::getline(stream, token, '|')) {
					if (token == "RED") {
						curr_flag |=
							asa::structures::CaveLootCrate::Quality::RED;
					}
					else if (token == "YELLOW") {
						curr_flag |=
							asa::structures::CaveLootCrate::Quality::YELLOW;
					}
					if (token == "BLUE") {
						curr_flag |=
							asa::structures::CaveLootCrate::Quality::BLUE;
					}
					if (token == "ANY") {
						curr_flag |=
							asa::structures::CaveLootCrate::Quality::ANY;
					}
				}
				crates_[group].emplace_back(
					util::add_num_to_prefix(
						config_.prefix + "::DROP", num_station++),
					config_, asa::structures::CaveLootCrate(curr_flag));
			}
			group++;
		}
	}

	void CrateManager::register_slash_commands()
	{
		dpp::slashcommand crate_commands(
			"crates", "Controls all crate managers.", 0);

		// reroll command must only be registered once for the first crate
		// manager to be crated.
		if (!has_registered_reroll_command_) {
			dpp::command_option reroll_group(
				dpp::co_sub_command_group, "reroll", "Manage reroll mode test");

			const auto set(dpp::command_option(dpp::co_boolean, "toggle",
				"Whether to use reroll mode.", true));

			reroll_group.add_option(dpp::command_option(
				dpp::co_sub_command, "set", "Set reroll mode")
										.add_option(set));

			reroll_group.add_option(dpp::command_option(
				dpp::co_sub_command, "get", "Get current reroll mode"));
			crate_commands.add_option(reroll_group);

			has_registered_reroll_command_ = true;
			core::discord::register_slash_command(
				crate_commands, reroll_mode_callback);
		}
	}

	void CrateManager::reroll_mode_callback(const dpp::slashcommand_t& event)
	{
		auto cmd_data = event.command.get_command_interaction();

		auto& subcommand = cmd_data.options[0].options[0];

		if (subcommand.name == "get") {
			return event.reply(std::format("Reroll mode is currently off"));
		}

		const bool enable = subcommand.get_value<bool>(0);
		std::string as_string = enable ? "true" : "false";

		auto& reroll = config::bots::drops::reroll_mode;
		if (reroll.get() == enable) {
			return event.reply(
				std::format("`reroll mode` is already `{}`", as_string));
		}
		reroll.set(enable);
		event.reply(
			std::format("`reroll mode` has been changed to `{}`", as_string));
	}
}
