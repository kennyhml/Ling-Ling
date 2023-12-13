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

		int get_times_looted() const { return times_looted; }

	private:
		cv::Mat loot_crate();
		void teleport_to();

		bool can_default_teleport = false;
		int times_looted = 0;

		asa::structures::CaveLootCrate crate;
		asa::structures::Teleporter teleporter;
	};
}