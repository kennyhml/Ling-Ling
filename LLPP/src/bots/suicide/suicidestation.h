#pragma once
#include "../../core/basestation.h"
#include <asapp/structures/simplebed.h>

namespace llpp::bots::suicide
{
	class SuicideStation : public core::BaseStation
	{
	public:
		SuicideStation(std::string bed, std::string respawn_bed);

		core::StationResult complete() override;

	private:
		asa::structures::SimpleBed death_bed;
		asa::structures::SimpleBed respawn_bed;
	};

}