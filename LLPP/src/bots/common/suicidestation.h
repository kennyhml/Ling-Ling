#pragma once
#include "../../core/bedstation.h"

namespace llpp::bots
{
	class SuicideStation : public core::BedStation
	{
	public:
		SuicideStation(std::string t_bed_name, std::string t_respawn_bed);

		core::StationResult complete() override;

	private:
		BedStation respawn_station_;
	};

}