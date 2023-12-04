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

		core::StationResult Complete() override;

		void SetCanDefaultTeleport(bool canDefaultTeleport);
		void SetCooldown();

		int GetTimesLooted() const;

	private:
		cv::Mat LootCrate();
		void TeleportTo();

		bool canDefaultTeleport = false;
		int timesLooted = 0;

		asa::structures::CaveLootCrate crate;
		asa::structures::Teleporter teleporter;
	};
}