#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/simplebed.h>

namespace llpp::bots::suicide
{
	class SuicideStation : public core::LLPPBaseStation
	{

	public:
		SuicideStation(std::string bedName, std::string respawnBedName)
			: LLPPBaseStation(bedName, std::chrono::minutes(5)),
			  deathBed(bedName), respawnBed(respawnBedName){};

		const core::StationResult Complete() override;

	private:
		asa::structures::SimpleBed deathBed;
		asa::structures::SimpleBed respawnBed;
	};

}