#pragma once
#include "../../core/basestationmanager.h"
#include "lootcratestation.h"

namespace llpp::bots::drops
{
	using QualityFlags = int;

	class CrateManager : public core::BaseStationManager
	{
	public:
		CrateManager(std::string prefix, int numberOfStations,
			std::vector<std::vector<QualityFlags>> groupedCrates);

		virtual const bool CompleteReadyStations();
		virtual const bool IsReadyToRun();
		virtual std::chrono::minutes GetTimeLeftUntilReady();

	private:
		void SetGroupCooldown(std::vector<LootCrateStation>& group);
		std::vector<std::vector<LootCrateStation>> crateGroups;
	};
}