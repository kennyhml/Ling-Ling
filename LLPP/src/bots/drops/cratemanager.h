#pragma once
#include "../../core/basestationmanager.h"
#include "../suicide/suicidestation.h"
#include "lootcratestation.h"
#include <asapp/structures/simplebed.h>


#define UNDEFINED_TIME std::chrono::system_clock::time_point::min()

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
			std::chrono::system_clock::time_point lastLooted = UNDEFINED_TIME;

			void AddLooted();

			std::chrono::seconds GetAverageRespawnTime() const
			{
				return this->avgRespawnTime;
			}

		private:
			int timesLooted = 0;
			std::chrono::seconds avgRespawnTime;
		};

		bool CompleteReadyStations() override;
		bool IsReadyToRun() override;

		std::chrono::minutes GetTimeLeftUntilReady();
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