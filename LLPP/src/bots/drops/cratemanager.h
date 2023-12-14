#pragma once
#include "../../core/data/managedvar.h"
#include "../../core/istationmanager.h"
#include "../suicide/suicidestation.h"
#include "lootcratestation.h"
#include <asapp/structures/simplebed.h>
#include <dpp/dpp.h>

#define UNDEFINED_TIME std::chrono::system_clock::time_point::min()

namespace llpp::bots::drops
{
	using QualityFlags = int;

	class CrateManager : public core::IStationManager
	{
	public:
		CrateManager(std::string t_prefix,
			std::vector<std::vector<QualityFlags>> t_grouped_crates,
			std::chrono::minutes t_interval,
			suicide::SuicideStation* t_suicide = nullptr);

		bool run() override;
		bool is_ready_to_run() const override;
		std::chrono::minutes get_time_left_until_ready() const;

	public:
		struct CrateGroupStatistics
		{
		public:
			std::chrono::system_clock::time_point last_looted = UNDEFINED_TIME;

			void add_looted();

			int get_times_looted() const { return times_looted; }
			std::chrono::seconds get_avg_respawn() const { return avg_respawn; }

		private:
			int times_looted = 0;
			std::chrono::seconds avg_respawn;
		};
		std::vector<CrateGroupStatistics> stats_per_group;

		static core::data::ManagedVar<bool> get_reroll_mode();

	private:
		const std::string prefix;

		asa::structures::SimpleBed align_bed;
		asa::structures::Teleporter dropoff_tp;
		asa::structures::Container dropoff_vault;

		suicide::SuicideStation* suicide;

		float last_known_vault_fill_level = 0.f;
		inline static bool has_registered_reroll_command = false;
		std::vector<std::vector<LootCrateStation>> crates;

	private:
		static void reroll_mode_callback(const dpp::slashcommand_t&);

		void dropoff_items(float& filledLevelOut);
		void teleport_to_dropoff();
		void run_all_stations(bool& wasAnyLootedOut);
		void access_bed_above();
		void spawn_on_align_bed();
		void register_slash_commands();
		void set_group_cooldown(std::vector<LootCrateStation>& group);
		void populate_groups(
			const std::vector<std::vector<QualityFlags>>& groups,
			std::chrono::minutes interval);
	};
}