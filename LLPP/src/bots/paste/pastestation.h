#pragma once
#include "../../core/station.h"
#include <asapp/entities/dinoent.h>
#include <asapp/entities/localplayer.h>
#include <asapp/items/items.h>
#include <asapp/structures/simplebed.h>
#include <dpp/colors.h>
#include <memory>

namespace llpp::bots::paste
{
	class PasteStation final : public core::LLPPBaseStation
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
			this->SetCompleted();
			auto duration = std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now() - start);

			core::StationResult resultData(true, duration, {}, nullptr);
			auto embed = this->CreateResultEmbed(resultData);
			resultData.discordEmbed = std::make_unique<dpp::embed>(embed);
			return resultData;
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
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			asa::entities::gLocalPlayer->TurnLeft(
				145, std::chrono::milliseconds(300));

			asa::entities::gLocalPlayer->DepositIntoDedicatedStorage(nullptr);
			asa::entities::gLocalPlayer->TurnUp(30);
			asa::entities::gLocalPlayer->DepositIntoDedicatedStorage(nullptr);
		}

		const dpp::embed CreateResultEmbed(const core::StationResult& result)
		{
			return dpp::embed()
				.set_color(dpp::colors::gray)
				.set_title(std::format(
					"Paste Station '{}' has been completed.", this->bed.name))
				.set_description(std::format(
					"The station was completed successfully {} times!",
					this->GetTimesCompleted()))
				.set_thumbnail(
					"https://static.wikia.nocookie.net/"
					"arksurvivalevolved_gamepedia/images/0/03/"
					"Cementing_Paste.png/revision/latest?cb=20180801020251")
				.add_field("Time taken:",
					std::to_string(result.timeTaken.count()) + " seconds", true)
				.add_field("Paste obtained:", "300", true)
				.add_field("Next completion:",
					std::to_string(this->completionInterval.count()) +
						" minutes",
					true);
		}
	};
}