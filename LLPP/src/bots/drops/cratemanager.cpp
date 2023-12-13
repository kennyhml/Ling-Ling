#include "cratemanager.h"
#include "../../common/util.h"
#include "../../core/discord.h"
#include "embeds.h"
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
	CrateManager::CrateManager(std::string t_prefix,
		std::vector<std::vector<QualityFlags>> t_grouped_crates,
		std::chrono::minutes t_interval, suicide::SuicideStation* t_suicide)
		: prefix(t_prefix), align_bed(t_prefix + "ALIGN"),
		  dropoff_tp(t_prefix + "DROPOFF"),
		  dropoff_vault(t_prefix + "DROPOFF", 350), suicide(t_suicide)
	{
		populate_groups(t_grouped_crates, t_interval);
	}

	void CrateManager::CrateGroupStatistics::add_looted()
	{
		const auto now = std::chrono::system_clock::now();
		std::chrono::seconds elapsedTime =
			(last_looted != UNDEFINED_TIME)
				? util::get_elapsed<std::chrono::seconds>(last_looted)
				: std::chrono::minutes(30);

		avg_respawn = ((avg_respawn * times_looted) + elapsedTime) /
					  static_cast<int>(times_looted + 1);

		times_looted++;
		last_looted = now;
	}

	bool CrateManager::run()
	{
		if (!is_ready_to_run()) {
			return false;
		}

		auto start = std::chrono::system_clock::now();
		spawn_on_align_bed();

		bool wasAnyLooted = false;
		run_all_stations(wasAnyLooted);

		teleport_to_dropoff();
		if (wasAnyLooted) {
			dropoff_items(last_known_vault_fill_level);
		}

		access_bed_above();
		if (suicide) {
			suicide->complete();
		}

		send_summary_embed(prefix,
			util::get_elapsed<std::chrono::seconds>(start), stats_per_group,
			last_known_vault_fill_level * 100,
			std::chrono::system_clock::now() +
				crates[0][0].get_completion_interval());
		return true;
	}

	void CrateManager::set_group_cooldown(std::vector<LootCrateStation>& group)
	{
		for (auto& station : group) {
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

	void CrateManager::run_all_stations(bool& anyLooted)
	{
		anyLooted = false;
		bool canDefaultTeleport = true;

		int i = 0;
		for (auto& group : crates) {
			for (auto& station : group) {
				station.set_can_default_teleport(canDefaultTeleport);
				auto result = station.complete();

				if (result.success) {
					anyLooted = true;
					stats_per_group[i].add_looted();
					set_group_cooldown(group);
					canDefaultTeleport = (&station == &group.back());
					break;
				}
				else {
					canDefaultTeleport = true;
				}
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

		for (size_t i = 0; i < groups.size(); i++) {
			for (QualityFlags qualities : groups[i]) {
				std::string name = util::add_num_to_prefix(
					prefix + "DROP", ++n);
				auto station = LootCrateStation(name, qualities, interval);
				crates[i].push_back(station);
			}
		}
	}
}
