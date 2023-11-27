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
		PasteStation(std::string name, PasteStation* prev = nullptr,
			PasteStation* next = nullptr)
			: LLPPBaseStation(name, std::chrono::minutes(90)),
			  bed(asa::structures::SimpleBed(name)), prev(prev), next(next)
		{
			for (int i = 0; i < 3; i++) {
				this->achatinas[i] = std::make_unique<asa::entities::DinoEnt>(
					std::format("ACHATINA0{}", i + 1));
			}
		};

		const PasteStation* GetPrev() { return this->prev; };
		const PasteStation* GetNext() { return this->next; };

		virtual const core::StationResult Complete() override;

	private:
		PasteStation* prev;
		PasteStation* next;

		std::array<std::unique_ptr<asa::entities::DinoEnt>, 3> achatinas;
		asa::structures::SimpleBed bed;

		bool EmptyAchatina(int index);
		void EmptyAllAchatinas();
		void DepositPasteIntoDedi();
	};
}