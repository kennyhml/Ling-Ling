#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/cavelootcrate.h>
#include <asapp/structures/teleporter.h>

namespace llpp::bots::drops
{
	class LootCrateStation final : public core::LLPPBaseStation
	{

	public:
		LootCrateStation(
			std::string name, asa ::structures::CaveLootCrate crate)
			: LLPPBaseStation(name, std::chrono::minutes(5)), crate(crate),
			  teleporter(asa ::structures::Teleporter(name)){};

		virtual const core::StationResult Complete() override;

		void SetCanDefaultTeleport(bool canDefaultTeleport);
		void SetCooldown();

	private:
		void FindDropoffBed();
		cv::Mat LootCrate();

		bool canDefaultTeleport = false;

		asa::structures::CaveLootCrate crate;
		asa::structures::Teleporter teleporter;
	};
}