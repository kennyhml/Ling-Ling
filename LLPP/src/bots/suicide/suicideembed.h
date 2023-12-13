#pragma once
#include "../../core/stationresult.h"
#include <string>

namespace llpp::bots::suicide
{
	void send_suicided_embed(const core::StationResult&,
		const std::string& at_bed, const std::string& respawning_at);
}