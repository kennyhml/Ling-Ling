#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/cavelootcrate.h>
#include <asapp/structures/teleporter.h>

namespace llpp::bots::drops
{
	class LootCrateStation final : public core::LLPPBaseStation
	{

	public:
		LootCrateStation(std::string name,
			asa ::structures::CaveLootCrate crate,
			std::chrono::minutes interval)
			: LLPPBaseStation(name, interval), crate(crate),
			  teleporter(asa ::structures::Teleporter(name)){};

		virtual const core::StationResult Complete() override;

		void SetCanDefaultTeleport(bool canDefaultTeleport);
		void SetCooldown();

	private:
		cv::Mat LootCrate();

		bool canDefaultTeleport = false;

		asa::structures::CaveLootCrate crate;
		asa::structures::Teleporter teleporter;
	};
}