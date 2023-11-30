#pragma once
#include "../../core/basestation.h"
#include <asapp/entities/dinoent.h>
#include <asapp/structures/simplebed.h>
#include <memory>

namespace llpp::bots::paste
{
	class PasteStation final : public core::LLPPBaseStation
	{
	public:
		PasteStation(std::string name)
			: LLPPBaseStation(name, std::chrono::minutes(30)),
			  bed(asa::structures::SimpleBed(name))
		{
			for (int i = 0; i < 3; i++) {
				this->achatinas[i] = std::make_unique<asa::entities::DinoEnt>(
					std::format("ACHATINA0{}", i + 1));
			}
		};

		virtual const core::StationResult Complete() override;

	private:
		std::array<std::unique_ptr<asa::entities::DinoEnt>, 3> achatinas;
		asa::structures::SimpleBed bed;

		bool EmptyAchatina(int index);
		void EmptyAllAchatinas();
		void DepositPasteIntoDedi();
	};
}