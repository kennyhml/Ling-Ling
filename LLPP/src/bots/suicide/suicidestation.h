#pragma once
#include "../../core/bedstation.h"
#include <asapp/structures/simplebed.h>

namespace llpp::bots::suicide
{
	class SuicideStation : public core::BedStation
	{
	public:
		SuicideStation(std::string bed, std::string respawn_bed);

		core::StationResult complete() override;

	private:
		asa::structures::SimpleBed respawn_bed;
	};

}