#pragma once
#include "../../core/stationresult.h"
#include <asapp/entities/dinoent.h>
#include <format>

namespace llpp::bots::paste
{
	void SendSuccessEmbed(const core::StationResult& data);
	void SendAchatinaNotAccessible(
		const std::string& stationName, asa::entities::DinoEnt* achatina);
	void SendAchatinaNotProducing();
	void SendDedisFull();
}