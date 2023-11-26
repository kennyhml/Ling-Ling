#pragma once
#include "../../core/station.h"
#include <asapp/entities/dinoent.h>
#include <asapp/entities/localplayer.h>
#include <asapp/items/items.h>
#include <asapp/structures/simplebed.h>
#include <memory>

namespace llpp::bots::paste
{
	class PasteStation : public core::LLPPBaseStation
	{
	public:
		PasteStation(asa::structures::SimpleBed bed)
			: bed(bed), LLPPBaseStation(bed.name, std::chrono::minutes(90))
		{
			for (int i = 0; i < 3; i++) {
				std::ostringstream oss;
				oss << std::setw(2) << std::setfill('0') << i;
				this->achatinas[i] = std::make_unique<asa::entities::DinoEnt>(
					"Achatina" + oss.str());
			}
		};

		std::array<std::unique_ptr<asa::entities::DinoEnt>, 3> achatinas;
		asa::structures::SimpleBed bed;

		virtual const core::StationResult Complete() override
		{
			auto start = std::chrono::system_clock::now();

			asa::entities::gLocalPlayer->FastTravelTo(&this->bed);

			asa::entities::gLocalPlayer->Crouch();
			asa::entities::gLocalPlayer->TurnDown(
				12, std::chrono::milliseconds(300));

			for (int i = 1; i <= 3; i++) {
				this->EmptyAchatina(i % 3);
				if (i < 3) {
					asa::entities::gLocalPlayer->TurnRight(i == 1 ? 35 : -70);
				}
			}
			this->DepositPasteIntoDedi();
			auto duration = std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now() - start);

			return core::StationResult(true, duration, {}, nullptr);
		}

	private:
		void EmptyAchatina(int index)
		{
			asa::entities::DinoEnt* snail = this->achatinas[index].get();
			asa::entities::gLocalPlayer->Access(snail);

			auto firstSlot = snail->inventory->slots[0];
			if (firstSlot.HasItem(asa::items::resources::achatinaPaste)) {
				snail->inventory->TakeSlot(0);
			}
			snail->Exit();
		}

		void DepositPasteIntoDedi()
		{
			asa::entities::gLocalPlayer->TurnLeft(55);
			asa::entities::gLocalPlayer->TurnLeft(
				90, std::chrono ::milliseconds(1000));

			asa::entities::gLocalPlayer->DepositIntoDedicatedStorage(nullptr);
			asa::entities::gLocalPlayer->TurnUp(30);
			asa::entities::gLocalPlayer->DepositIntoDedicatedStorage(nullptr);
		}
	};
}