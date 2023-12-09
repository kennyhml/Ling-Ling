#include "pastestation.h"
#include "../../common/util.h"
#include "embeds.h"
#include <algorithm>
#include <asapp/entities/exceptions.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/exceptions.h>

using namespace llpp::bots::paste;

PasteStation::PasteStation(std::string name, std::chrono::minutes interval)
	: BaseStation(name, interval), bed(asa::structures::SimpleBed(name)){};

llpp::core::StationResult PasteStation::Complete()
{
	auto start = std::chrono::system_clock::now();

	asa::entities::gLocalPlayer->FastTravelTo(bed);
	EmptyAllAchatinas();
	int pasteObtained = DepositPasteIntoDedi();
	SetCompleted();

	auto duration = util::GetElapsed<std::chrono::seconds>(start);
	core::StationResult resultData(
		this, true, duration, { { "Achatina Paste", pasteObtained } });

	SendSuccessEmbed(resultData);
	return resultData;
}

bool PasteStation::EmptyAchatina(int index)
{
	asa::entities::DinoEnt& snail = this->achatinas[index];
	try {
		asa::entities::gLocalPlayer->Access(snail);
	}
	catch (const asa::entities::exceptions::EntityNotAccessed& e) {
		SendAchatinaNotAccessible(name, e.GetEntity()->GetName());
		return true;
	}

	auto& slot = snail.inventory->slots[0];
	bool hasPaste = slot.HasItem(asa::items::resources::achatinaPaste);
	if (hasPaste) {
		snail.inventory->TakeSlot(0);
	}
	snail.Exit();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	return hasPaste;
}

void PasteStation::EmptyAllAchatinas()
{
	std::cout << "[+] Emptying the achatinas..." << std::endl;
	this->EmptyAchatina(4);
	do {
		asa::entities::gLocalPlayer->TurnLeft(45);
	} while (!this->EmptyAchatina(3));
	do {
		asa::entities::gLocalPlayer->TurnRight(90);
	} while (!this->EmptyAchatina(5));

	asa::entities::gLocalPlayer->Crouch();
	asa::entities::gLocalPlayer->TurnDown(12, std::chrono::milliseconds(300));

	this->EmptyAchatina(2);
	do {
		asa::entities::gLocalPlayer->TurnLeft(45);
	} while (!this->EmptyAchatina(1));

	do {
		asa::entities::gLocalPlayer->TurnLeft(45);
	} while (!this->EmptyAchatina(0));
}

int PasteStation::DepositPasteIntoDedi()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	asa::entities::gLocalPlayer->TurnLeft(135, std::chrono::milliseconds(300));

	int amount = 0;
	asa::entities::gLocalPlayer->DepositIntoDedicatedStorage(
		asa::items::resources::achatinaPaste, &amount);
	return amount;
}
