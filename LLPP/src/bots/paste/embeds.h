#pragma once
#include "../../core/stationresult.h"
#include <asapp/entities/dinoent.h>
#include <format>

namespace llpp::bots::paste
{
	void SendSuccessEmbed(const core::StationResult& data);
	void SendAchatinaNotAccessible(
		const std::string& stationName, const std::string& achatina);

	void SendAchatinaNotProducing();
	void SendDedisFull();
	void SendPasteNotDeposited();
}