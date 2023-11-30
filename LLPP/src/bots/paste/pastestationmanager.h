#pragma once
#include "../../core/basestationmanager.h"
#include "../render/renderstation.h"
#include "pastestation.h"

namespace llpp::bots::paste
{

	class PasteStationManager final : public core::BaseStationManager
	{
	public:
		PasteStationManager(std::string prefix, int numOfStations)
			: BaseStationManager(prefix, numOfStations)
		{
			for (int i = 0; i < numOfStations; i++) {
				std::string name = this->CreateStationName(prefix, i + 1);
				this->stations.push_back(std::make_unique<PasteStation>(name));
			}
		}

		virtual const bool CompleteReadyStations() override;
		virtual const bool IsReadyToRun() override;
		virtual std::chrono::minutes GetTimeLeftUntilReady() override;


	private:
		std::vector<std::unique_ptr<PasteStation>> stations;
		render::RenderStation renderStation{ "PASTE::RENDER::SRC",
			std::chrono::seconds(45) };
	};

}