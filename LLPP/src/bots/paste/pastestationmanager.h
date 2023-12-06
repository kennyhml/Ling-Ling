#pragma once
#include "../../core/basestationmanager.h"
#include "../render/renderstation.h"
#include "pastestation.h"
#include <memory>

namespace llpp::bots::paste
{

	class PasteStationManager final : public core::BaseStationManager
	{
	public:
		PasteStationManager(std::string prefix, int numOfStations,
			std::chrono::minutes interval);

		bool CompleteReadyStations() override;
		bool IsReadyToRun() override;
		std::chrono::minutes GetTimeLeftUntilReady() override;


	private:
		std::vector<std::unique_ptr<PasteStation>> stations;
		render::RenderStation renderStation{ "PASTE::RENDER::SRC",
			std::chrono::seconds(10) };
	};

}