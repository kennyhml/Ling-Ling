#include "pastestation.h"
#include "pasteembeds.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/exceptions.h>

using namespace llpp::bots::paste;

const llpp::core::StationResult PasteStation::Complete()
{
	auto start = std::chrono::system_clock::now();

	asa::entities::gLocalPlayer->FastTravelTo(&this->bed);
	this->EmptyAllAchatinas();
	this->DepositPasteIntoDedi();
	this->SetCompleted();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now() - start);

	core::StationResult resultData(
		this, true, this->GetTimesCompleted(), duration, {});

	SendSuccessEmbed(resultData);
	return resultData;
}

bool PasteStation::EmptyAchatina(int index)
{
	asa::entities::DinoEnt* snail = this->achatinas[index].get();
	try {
		asa::entities::gLocalPlayer->Access(snail);
	}
	catch (asa::interfaces::exceptions::InterfaceNotOpenedError e) {
		std::cerr << "Failed to open " << snail->GetName() << std::endl;
		SendAchatinaNotAccessible(this->name, snail);
	}

	auto slot = snail->inventory->slots[0];
	bool hasPaste = slot.HasItem(asa::items::resources::achatinaPaste);
	if (hasPaste) {
		snail->inventory->TakeSlot(0);
	}
	snail->Exit();
	return hasPaste;
}

void PasteStation::EmptyAllAchatinas()
{
	std::cout << "[+] Emptying the achatinas..." << std::endl;
	asa::entities::gLocalPlayer->Crouch();
	asa::entities::gLocalPlayer->TurnDown(12, std::chrono::milliseconds(300));

	this->EmptyAchatina(1);
	std::cout << "\t[-] Achatina 1 - Done." << std::endl;
	do {
		asa::entities::gLocalPlayer->TurnRight(35);
	} while (!this->EmptyAchatina(2));
	std::cout << "\t[-] Achatina 2 - Done." << std::endl;

	do {
		asa::entities::gLocalPlayer->TurnLeft(70);
	} while (!this->EmptyAchatina(0));
	std::cout << "\t[-] Achatina 3 - Done." << std::endl;
}

void PasteStation::DepositPasteIntoDedi()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	asa::entities::gLocalPlayer->TurnLeft(145, std::chrono::milliseconds(300));

	asa::entities::gLocalPlayer->DepositIntoDedicatedStorage(nullptr);
}
