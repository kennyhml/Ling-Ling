#include "cratemanager.h"

using namespace llpp::bots::drops;

CrateManager::CrateManager(std::string prefix, int numberOfStations,
	std::vector<std::vector<QualityFlags>> groupedCrates)
	: BaseStationManager(prefix, numberOfStations)
{
	crateGroups.resize(groupedCrates.size());
	int stationNumber = 0;

	for (size_t i = 0; i < groupedCrates.size(); i++) {
		for (QualityFlags crateQualities : groupedCrates[i]) {
			auto crate = asa::structures::CaveLootCrate(crateQualities);
			std::string name = CreateStationName(stationNumber + 1);
			auto station = LootCrateStation(name, crate);

			this->crateGroups[i].push_back(station);
			stationNumber += 1;
		}
	}
}


const bool CrateManager::CompleteReadyStations()
{
	bool canDefaultTeleport = true;

	for (auto& group : this->crateGroups) {
		for (auto& station : group) {
			station.SetCanDefaultTeleport(canDefaultTeleport);
			auto result = station.Complete();

			if (result.success) {
				canDefaultTeleport = (&station == &group.back());
				break;
			}
			else {
				canDefaultTeleport = true;
			}
		}
	}
	return true;
}

void CrateManager::SetGroupCooldown(std::vector<LootCrateStation>& group)
{
	for (auto& station : group) {
		station.SetCanDefaultTeleport(false);
		station.SetCooldown();
	}
}
const bool CrateManager::IsReadyToRun() { return false; }

std::chrono::minutes llpp::bots::drops::CrateManager::GetTimeLeftUntilReady()
{
	return std::chrono::minutes(0);
}