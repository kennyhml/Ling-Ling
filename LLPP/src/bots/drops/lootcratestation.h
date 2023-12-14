#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/cavelootcrate.h>
#include <asapp/structures/teleporter.h>

namespace llpp::bots::drops
{
	class LootCrateStation final : public core::BaseStation
	{
	public:
		LootCrateStation(std::string name, asa::structures::CaveLootCrate crate,
			std::chrono::minutes interval);

		core::StationResult complete() override;

		void set_can_default_teleport(bool can_default_teleport);
		void set_cooldown();
		void set_fully_loot(bool fully_loot);

		int get_times_looted() const { return times_looted; }

	private:
		bool fully_loot = true;

		cv::Mat loot_crate();
		void teleport_to();
		bool has_buff_wait_expired() const;

		bool can_default_teleport = false;
		int times_looted = 0;

		bool is_crate_up = false;
		std::chrono::system_clock::time_point last_discovered_up;
		std::chrono::minutes max_buff_wait_time = std::chrono::minutes(15);

		asa::structures::CaveLootCrate crate;
		asa::structures::Teleporter teleporter;
	};
}