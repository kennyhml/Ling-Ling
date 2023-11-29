#include "cratemanager.h"
#include <asapp/entities/localplayer.h>

using namespace llpp::bots::drops;

CrateManager::CrateManager(std::string prefix, int numberOfStations,
	std::vector<std::vector<QualityFlags>> groupedCrates,
	std::chrono::minutes interval)
	: BaseStationManager(prefix, numberOfStations), alignBed(prefix + "ALIGN"),
	  dropoffBed(prefix + "DROPOFF"), dropoffTeleporter(prefix + "DROPOFF"),
	  grindablesVault(prefix + " GRINDABLES"),
	  blueprintsVault(prefix + "BLUEPRINTS"), miscVault(prefix + "MISC")
{
	crateGroups.resize(groupedCrates.size());
	int stationNumber = 0;

	for (size_t i = 0; i < groupedCrates.size(); i++) {
		for (QualityFlags crateQualities : groupedCrates[i]) {
			auto crate = asa::structures::CaveLootCrate(crateQualities);
			std::string name = CreateStationName(stationNumber + 1);
			auto station = LootCrateStation(name, crate, interval);

			this->crateGroups[i].push_back(station);
			stationNumber += 1;
		}
	}
}

const bool CrateManager::CompleteReadyStations()
{
	asa::entities::gLocalPlayer->FastTravelTo(&this->alignBed);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	asa::entities::gLocalPlayer->Crouch();

	bool canDefaultTeleport = true;
	for (auto& group : this->crateGroups) {
		for (auto& station : group) {
			station.SetCanDefaultTeleport(canDefaultTeleport);
			auto result = station.Complete();

			if (result.success) {
				this->SetGroupCooldown(group);
				canDefaultTeleport = (&station == &group.back());
				break;
			}
			else {
				canDefaultTeleport = true;
			}
		}
	}
	this->DropoffItems();
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

void CrateManager::FindDropoffBed()
{
	asa::entities::gLocalPlayer->StandUp();
	asa::entities::gLocalPlayer->LookAllTheWayUp();
	std::this_thread::sleep_for(std::chrono::seconds(1));

	if (this->dropoffDownTurnAmount != -1 &&
		this->dropoffRightTurnAmount != -1) {
		std::cout << "[+] Using cached values from previous dropoff."
				  << std::endl;
		asa::entities::gLocalPlayer->TurnDown(this->dropoffDownTurnAmount);
		asa::entities::gLocalPlayer->TurnRight(this->dropoffRightTurnAmount);
		return;
	}

	int totalRight = 0;
	int totalDown = 0;

	const int rightTurns = 6;
	const int downTurns = 3;
	std::chrono::milliseconds delayPerTurn = std::chrono::milliseconds(300);

	int downTurned = 0;
	bool found = false;
	while (!asa::entities::gLocalPlayer->CanAccessBed()) {

		for (int i = 0; i < rightTurns; i++) {
			int amount = 360 / rightTurns;
			asa::entities::gLocalPlayer->TurnRight(amount, delayPerTurn);
			totalRight += amount;

			if (asa::entities::gLocalPlayer->CanAccessBed()) {
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}

		totalRight = 0;
		if (downTurned > downTurns) {
			throw std::runtime_error("Failed to find bed");
		}
		asa::entities::gLocalPlayer->TurnDown(20, delayPerTurn);
		downTurned++;
		totalDown += 20;
	}

	std::cout << "[+] Found a spot to access bed. Right turn: " << totalRight
			  << ",down turn: " << totalDown << std::endl;
}

void CrateManager::DropoffItems()
{
	asa::entities::gLocalPlayer->TeleportTo(&this->dropoffTeleporter, true);
	std::this_thread::sleep_for(std::chrono::seconds(30));
	this->FindDropoffBed();

	asa::entities::gLocalPlayer->LayOn(&this->dropoffBed);
	std::this_thread::sleep_for(std::chrono::seconds(10));
	asa::entities::gLocalPlayer->GetUp();

	asa::entities::gLocalPlayer->Access(&this->blueprintsVault);
	asa::entities::gLocalPlayer->inventory->TransferAll("Blueprint");
	this->blueprintsVault.inventory->Close();
	std::this_thread::sleep_for(std::chrono::seconds(2));

	asa::entities::gLocalPlayer->TurnLeft(50);
	asa::entities::gLocalPlayer->Access(&this->grindablesVault);
	asa::entities::gLocalPlayer->inventory->TransferAll("riot");
	asa::entities::gLocalPlayer->inventory->TransferAll("assault");
	asa::entities::gLocalPlayer->inventory->TransferAll("pistol");
	asa::entities::gLocalPlayer->inventory->TransferAll("ghilie");
	this->grindablesVault.inventory->Close();
	std::this_thread::sleep_for(std::chrono::seconds(2));


	asa::entities::gLocalPlayer->TurnRight(50);
	asa::entities::gLocalPlayer->TurnRight(50);
	asa::entities::gLocalPlayer->Access(&this->miscVault);
	asa::entities::gLocalPlayer->inventory->TransferAll();
	this->miscVault.inventory->Close();
	std::this_thread::sleep_for(std::chrono::seconds(2));
}
