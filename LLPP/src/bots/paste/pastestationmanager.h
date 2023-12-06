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
		PasteStationManager(std::string prefix, int numOfStations);

		bool CompleteReadyStations() override;
		bool IsReadyToRun() override;
		std::chrono::minutes GetTimeLeftUntilReady() override;

		std::vector<std::unique_ptr<PasteStation>> stations;

	private:
		std::vector<PasteStation> CreateStations(
			std::string prefix, int number);
		render::RenderStation renderStation{ "PASTE::RENDER::SRC",
			std::chrono::seconds(45) };
	};

}