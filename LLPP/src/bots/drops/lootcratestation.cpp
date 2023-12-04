#include "lootcratestation.h"
#include "../../common/util.h"
#include "embeds.h"
#include <asapp/entities/localplayer.h>

using namespace llpp::bots::drops;


LootCrateStation::LootCrateStation(std::string name,
	asa::structures::CaveLootCrate crate, std::chrono::minutes interval)
	: BaseStation(name, interval), crate(crate),
	  teleporter(asa ::structures::Teleporter(name)){};

llpp::core::StationResult LootCrateStation::Complete()
{
	auto start = std::chrono::system_clock::now();
	asa::structures::CaveLootCrate::Quality quality;
	cv::Mat loot;

	this->TeleportTo();
	bool isDropUp = asa::entities::gLocalPlayer->CanAccess(this->crate);

	if (isDropUp) {
		quality = this->crate.GetCrateQuality();
		loot = this->LootCrate();
	}

	this->SetCompleted();
	auto timePassed = util::GetElapsed<std::chrono::seconds>(start);
	core::StationResult result(this, isDropUp, timePassed, {});

	if (result.success) {
		SendSuccessEmbed(result, loot, quality);
	}
	return result;
}

void LootCrateStation::SetCanDefaultTeleport(bool canDefaultTeleport)
{
	this->canDefaultTeleport = canDefaultTeleport;
}

void LootCrateStation::SetCooldown()
{
	this->lastCompleted = std::chrono::system_clock::now();
}

cv::Mat LootCrateStation::LootCrate()
{
	asa::entities::gLocalPlayer->Access(this->crate);
	auto loot = asa::window::Screenshot({ 1193, 227, 574, 200 });

	do {
		this->crate.inventory->TransferAll();
	} while (!util::Await([this]() { return !this->crate.inventory->IsOpen(); },
		std::chrono::seconds(6)));

	return loot;
}

void LootCrateStation::TeleportTo()
{
	asa::entities::gLocalPlayer->TeleportTo(
		this->teleporter, this->canDefaultTeleport);

	if (!this->canDefaultTeleport) {
		asa::entities::gLocalPlayer->TurnUp(60, std::chrono::milliseconds(500));
	}
}


int LootCrateStation::GetTimesLooted() const { return this->timesLooted; }