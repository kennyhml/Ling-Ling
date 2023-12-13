#include "lootcratestation.h"
#include "../../common/util.h"
#include "embeds.h"
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
	LootCrateStation::LootCrateStation(std::string t_name,
		asa::structures::CaveLootCrate t_crate, std::chrono::minutes t_interval)
		: BaseStation(name, t_interval), crate(t_crate),
		  teleporter(asa ::structures::Teleporter(t_name)){};

	llpp::core::StationResult LootCrateStation::complete()
	{
		auto start = std::chrono::system_clock::now();
		auto quality = asa::structures::CaveLootCrate::Quality::ANY;
		cv::Mat loot;

		this->teleport_to();
		bool isDropUp = asa::entities::local_player->can_access(crate);

		if (isDropUp) {
			std::cout << "[+] Crate found, looting..." << std::endl;
			quality = crate.get_crate_quality();
			loot = loot_crate();
		}

		set_completed();
		auto timePassed = util::get_elapsed<std::chrono::seconds>(start);
		core::StationResult result(this, isDropUp, timePassed, {});

		if (result.success) {
			send_success_embed(result, loot, quality, ++times_looted);
		}
		return result;
	}

	void LootCrateStation::set_can_default_teleport(bool can_default_teleport)
	{
		this->can_default_teleport = can_default_teleport;
	}

	void LootCrateStation::set_cooldown()
	{
		last_completed = std::chrono::system_clock::now();
	}

	cv::Mat LootCrateStation::loot_crate()
	{
		asa::entities::local_player->access(this->crate);
		auto loot = asa::window::screenshot({ 1193, 227, 574, 200 });
		std::cout << "\t[-] Loot screenshot has been taken." << std::endl;
		do {
			this->crate.inventory->transfer_all();
		} while (
			!util::await([this]() { return !this->crate.inventory->is_open(); },
				std::chrono::seconds(6)));
		std::cout << "\t[-] The crate has been looted successfully."
				  << std::endl;
		return loot;
	}

	void LootCrateStation::teleport_to()
	{
		asa::entities::local_player->teleport_to(
			this->teleporter, this->can_default_teleport);

		if (!this->can_default_teleport) {
			asa::entities::local_player->turn_up(
				60, std::chrono::milliseconds(500));
		}
	}

}
