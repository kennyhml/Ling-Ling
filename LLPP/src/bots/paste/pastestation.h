#pragma once
#include "../../core/basestation.h"
#include <asapp/entities/dinoent.h>
#include <asapp/structures/simplebed.h>

namespace llpp::bots::paste
{
	class PasteStation final : public core::BaseStation
	{
	public:
		PasteStation(std::string name);

		core::StationResult Complete() override;

	private:
		std::array<asa::entities::DinoEnt, 3> achatinas{
			asa::entities::DinoEnt("ACHATINA01"),
			asa::entities::DinoEnt("ACHATINA02"),
			asa::entities::DinoEnt("ACHATINA03"),
		};
		asa::structures::SimpleBed bed;

		bool EmptyAchatina(int index);
		void EmptyAllAchatinas();
		void DepositPasteIntoDedi();
	};
}