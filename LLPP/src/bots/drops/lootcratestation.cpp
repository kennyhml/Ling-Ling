#include "lootcratestation.h"
#include "../../common/util.h"
#include "crateembeds.h"
#include <asapp/entities/localplayer.h>

const llpp::core::StationResult llpp::bots::drops::LootCrateStation::Complete()
{
	auto start = std::chrono::system_clock::now();

	asa::entities::gLocalPlayer->TeleportTo(
		&this->teleporter, this->canDefaultTeleport);

	if (!this->canDefaultTeleport) {
		asa::entities::gLocalPlayer->TurnUp(60, std::chrono::milliseconds(500));
	}

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

void llpp::bots::drops::LootCrateStation::SetCanDefaultTeleport(
	bool canDefaultTeleport)
{
	this->canDefaultTeleport = canDefaultTeleport;
}

void llpp::bots::drops::LootCrateStation::SetCooldown()
{
	this->lastCompleted = std::chrono::system_clock::now();
}

cv::Mat llpp::bots::drops::LootCrateStation::LootCrate()
{
	asa::entities::gLocalPlayer->Access(&this->crate);
	auto loot = asa::window::Screenshot({ 1193, 227, 574, 200 });

	do {
		this->crate.inventory->TransferAll();
	} while (!util::Await([this]() { return !this->crate.inventory->IsOpen(); },
		std::chrono::seconds(1)));
	return loot;
}
