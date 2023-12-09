#pragma once
#include "../../core/istationmanager.h"
#include "../suicide/suicidestation.h"
#include "lootcratestation.h"
#include <asapp/structures/simplebed.h>

#define UNDEFINED_TIME std::chrono::system_clock::time_point::min()

namespace llpp::bots::drops
{
	using QualityFlags = int;

	class CrateManager : public core::IStationManager
	{
	public:
		CrateManager(std::string prefix,
			std::vector<std::vector<QualityFlags>> groupedCrates,
			std::chrono::minutes interval,
			suicide::SuicideStation* suicide = nullptr);

		struct CrateGroupStatistics
		{
			std::chrono::system_clock::time_point lastLooted = UNDEFINED_TIME;

			void AddLooted();

			int GetTimesLooted() const { return timesLooted; }
			std::chrono::seconds GetAverageRespawnTime() const
			{
				return avgRespawnTime;
			}

		private:
			int timesLooted = 0;
			std::chrono::seconds avgRespawnTime;
		};

		bool Run() override;
		bool IsReadyToRun() override;

		std::chrono::minutes GetTimeLeftUntilReady();
		std::vector<CrateGroupStatistics> statisticsPerGroup;

	private:
		void DropoffItems(float& filledLevelOut);
		void TeleportToDropoff();
		void RunAllStations(bool& wasAnyLootedOut);
		void AccessBedAbove();
		void SpawnOnAlignBed();
		void PopulateGroups(
			const std::vector<std::vector<QualityFlags>>& groups,
			std::chrono::minutes stationInterval);

		asa::structures::SimpleBed alignBed;
		asa::structures::Teleporter dropoffTeleporter;
		asa::structures::Container dropoffVault;

		void SetGroupCooldown(std::vector<LootCrateStation>& group);
		std::vector<std::vector<LootCrateStation>> crateGroups;

		std::string prefix;
		float lastKnownVaultFillLevel = 0.f;
		suicide::SuicideStation* suicide;
	};
}