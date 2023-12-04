#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/simplebed.h>

namespace llpp::bots::suicide
{
	class SuicideStation : public core::BaseStation
	{
	public:
		SuicideStation(std::string bedName, std::string respawnBedName);

		core::StationResult Complete() override;

	private:
		asa::structures::SimpleBed deathBed;
		asa::structures::SimpleBed respawnBed;
	};

}