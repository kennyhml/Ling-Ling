#pragma once
#include "../../core/istationmanager.h"
#include "../render/renderstation.h"
#include "pastestation.h"
#include <memory>

namespace llpp::bots::paste
{
	class PasteManager final : public core::IStationManager
	{
	public:
		PasteManager(std::string prefix, int numOfStations,
			std::chrono::minutes interval);

		bool Run() override;
		bool IsReadyToRun() override;
		std::chrono::minutes GetTimeLeftUntilReady() override;

		const PasteStation* PeekStation(int index) const;

	private:
		std::vector<std::unique_ptr<PasteStation>> stations;
		render::RenderStation renderStation{ "PASTE::RENDER::SRC",
			std::chrono::seconds(10) };

		void RegisterSlashEvents();
	};
}