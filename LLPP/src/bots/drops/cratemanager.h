#pragma once
#include "../../core/basestationmanager.h"
#include "../suicide/suicidestation.h"
#include "lootcratestation.h"
#include <asapp/structures/simplebed.h>

namespace llpp::bots::drops
{
	using QualityFlags = int;

	class CrateManager : public core::BaseStationManager
	{
	public:
		CrateManager(std::string prefix, int numberOfStations,
			std::vector<std::vector<QualityFlags>> groupedCrates,
			std::chrono::minutes interval,
			suicide::SuicideStation* suicide = nullptr);

		struct CrateGroupStatistics
		{
			std::chrono::system_clock::time_point lastLooted =
				std::chrono::system_clock::time_point::min();

			int timesLooted = 0;
			std::chrono::seconds averageRespawnTime = std::chrono::minutes(15);

			void AddLooted();
		};

		virtual const bool CompleteReadyStations();
		virtual const bool IsReadyToRun();
		virtual std::chrono::minutes GetTimeLeftUntilReady();

		std::vector<CrateGroupStatistics> statisticsPerGroup;

	private:
		void DropoffItems();
		void TeleportToDropoff();
		void RunAllStations(bool& wasAnyLootedOut);
		void AccessBedAbove();
		void SpawnOnAlignBed();

		int dropoffRightTurnAmount = -1;
		int dropoffDownTurnAmount = -1;

		asa::structures::SimpleBed alignBed;
		asa::structures::Teleporter dropoffTeleporter;
		asa::structures::Container dropoffVault;

		void SetGroupCooldown(std::vector<LootCrateStation>& group);
		std::vector<std::vector<LootCrateStation>> crateGroups;

		suicide::SuicideStation* suicide;
	};
}