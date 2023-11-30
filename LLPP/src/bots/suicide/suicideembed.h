#pragma once
#include "../../core/stationresult.h"
#include <string>

namespace llpp::bots::suicide
{
	void SendSuicided(const core::StationResult&, const std::string& atBed,
		const std::string& respawningAt);
}