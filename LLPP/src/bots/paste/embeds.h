#pragma once
#include "../../core/stationresult.h"
#include <asapp/entities/dinoent.h>
#include <format>

namespace llpp::bots::paste
{
	void send_success_embed(const core::StationResult& data);
	void send_achatina_not_accessible(
		const std::string& stationName, const std::string& achatina);

	void SendAchatinaNotProducing();
	void SendDedisFull();
	void SendPasteNotDeposited();
}