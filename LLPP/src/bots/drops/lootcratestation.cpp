#include "lootcratestation.h"
#include "../../common/util.h"
#include "crateembeds.h"
#include <asapp/entities/localplayer.h>

const llpp::core::StationResult llpp::bots::drops::LootCrateStation::Complete()
{
	auto start = std::chrono::system_clock::now();

	asa::entities::gLocalPlayer->TeleportTo(&this->teleporter, true);

	// crate wasnt up
	if (!asa::entities::gLocalPlayer->CanAccess(&this->crate)) {
		return core::StationResult(this, false, this->GetTimesCompleted(),
			std::chrono::seconds(0), {});
	}

	auto quality = this->crate.GetCrateQuality();
	cv::Mat loot = this->LootCrate();
	this->SetCompleted();

	auto duration = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now() - start);
	core::StationResult resultData(
		this, true, this->GetTimesCompleted(), duration, {});

	SendSuccessEmbed(resultData, loot, quality);
	return resultData;
}

cv::Mat llpp::bots::drops::LootCrateStation::LootCrate()
{
	asa::entities::gLocalPlayer->Access(&this->crate);
	auto loot = asa::window::Screenshot({ 1193, 227, 574, 200 });

	do {
		this->crate.inventory->TransferAll();
	} while (!util::Await([this]() { return !this->crate.inventory->IsOpen(); },
		std::chrono::seconds(5)));
	return loot;
}

void llpp::bots::drops::LootCrateStation::FindDropoffBed()
{
	asa::entities::gLocalPlayer->LookAllTheWayUp();

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
