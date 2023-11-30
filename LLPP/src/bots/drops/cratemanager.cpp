#include "cratemanager.h"
#include <asapp/entities/localplayer.h>

using namespace llpp::bots::drops;

CrateManager::CrateManager(std::string prefix, int numberOfStations,
	std::vector<std::vector<QualityFlags>> groupedCrates,
	std::chrono::minutes interval, suicide::SuicideStation* suicide)
	: BaseStationManager(prefix, numberOfStations), alignBed(prefix + "ALIGN"),
	  dropoffTeleporter(prefix + "DROPOFF"), dropoffVault(prefix + "DROPOFF"),
	  suicide(suicide)
{
	crateGroups.resize(groupedCrates.size());
	statisticsPerGroup.resize(groupedCrates.size());
	int stationNumber = 0;

	for (size_t i = 0; i < groupedCrates.size(); i++) {
		for (QualityFlags crateQualities : groupedCrates[i]) {
			auto crate = asa::structures::CaveLootCrate(crateQualities);
			std::string name = CreateStationName(
				prefix + "DROP", stationNumber + 1);
			auto station = LootCrateStation(name, crate, interval);

			this->crateGroups[i].push_back(station);
			stationNumber += 1;
		}
	}
}

void CrateManager::CrateGroupStatistics::AddLooted()
{
	auto currentTime = std::chrono::system_clock::now();
	if (lastLooted != std::chrono::system_clock::time_point::min()) {
		auto timeSinceLastLooted =
			std::chrono::duration_cast<std::chrono::minutes>(
				currentTime - lastLooted);

		averageRespawnTime =
			(averageRespawnTime * timesLooted + timeSinceLastLooted) /
			static_cast<int>(timesLooted + 1);
	}
	timesLooted++;
	lastLooted = currentTime;
}

const bool CrateManager::CompleteReadyStations()
{
	if (!this->crateGroups[0][0].IsReady()) {
		return false;
	}
	this->SpawnOnAlignBed();

	bool wasAnyLooted = false;
	this->RunAllStations(wasAnyLooted);

	this->TeleportToDropoff();
	if (wasAnyLooted) {
		this->DropoffItems();
	}
	this->AccessBedAbove();
	if (this->suicide != nullptr) {
		this->suicide->Complete();
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


void CrateManager::RunAllStations(bool& anyLooted)
{
	anyLooted = false;
	bool canDefaultTeleport = true;

	int i = 0;
	for (auto& group : this->crateGroups) {
		for (auto& station : group) {
			station.SetCanDefaultTeleport(canDefaultTeleport);
			auto result = station.Complete();

			if (result.success) {
				anyLooted = true;
				this->statisticsPerGroup[i].AddLooted();
				this->SetGroupCooldown(group);
				canDefaultTeleport = (&station == &group.back());
				break;
			}
			else {
				canDefaultTeleport = true;
			}
		}
		i++;
	}
}

void CrateManager::DropoffItems()
{
	asa::entities::gLocalPlayer->TurnUp(50, std::chrono::milliseconds(500));
	asa::entities::gLocalPlayer->TurnRight(90);

	asa::entities::gLocalPlayer->Access(&this->dropoffVault);
	asa::entities::gLocalPlayer->inventory->TransferAll();
	this->dropoffVault.inventory->Close();
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

void CrateManager::TeleportToDropoff()
{
	asa::entities::gLocalPlayer->TeleportTo(&this->dropoffTeleporter);
	std::this_thread::sleep_for(std::chrono::seconds(10));
	asa::entities::gLocalPlayer->StandUp();
}

void CrateManager::AccessBedAbove()
{
	asa::entities::gLocalPlayer->LookAllTheWayUp();
	asa::entities::gLocalPlayer->Access(&this->alignBed);
}

void CrateManager::SpawnOnAlignBed()
{
	asa::entities::gLocalPlayer->FastTravelTo(&this->alignBed);
	asa::entities::gLocalPlayer->Crouch();
	asa::entities::gLocalPlayer->TurnDown(20);
}