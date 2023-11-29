#pragma once
#include "../../core/basestationmanager.h"
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
			std::chrono::minutes interval);

		virtual const bool CompleteReadyStations();
		virtual const bool IsReadyToRun();
		virtual std::chrono::minutes GetTimeLeftUntilReady();
		void DropoffItems();

	private:
		void FindDropoffBed();

		int dropoffRightTurnAmount = -1;
		int dropoffDownTurnAmount = -1;

		asa::structures::SimpleBed alignBed;
		asa::structures::SimpleBed dropoffBed;
		asa::structures::Teleporter dropoffTeleporter;

		asa::structures::Container grindablesVault;
		asa::structures::Container blueprintsVault;
		asa::structures::Container miscVault;

		void SetGroupCooldown(std::vector<LootCrateStation>& group);
		std::vector<std::vector<LootCrateStation>> crateGroups;
	};
}