#include "lootcratestation.h"
#include "../../common/util.h"
#include "embeds.h"
#include <asapp/entities/localplayer.h>

namespace llpp::bots::drops
{
	LootCrateStation::LootCrateStation(std::string t_name,
		asa::structures::CaveLootCrate t_crate, std::chrono::minutes t_interval)
		: BaseStation(t_name, t_interval), crate(t_crate),
		  teleporter(asa ::structures::Teleporter(t_name)){};

	llpp::core::StationResult LootCrateStation::complete()
	{
		auto start = std::chrono::system_clock::now();

		teleport_to();
		if (!asa::entities::local_player->can_access(crate)) {
			is_crate_up = false;
			set_completed();
			return core::StationResult(this, 0, std::chrono::seconds(0), {});
		}

		std::cout << "[+] Crate found..." << std::endl;
		if (!is_crate_up) {
			is_crate_up = true;
			last_discovered_up = std::chrono::system_clock::now();
		}

		if (has_buff_wait_expired()) {
			std::cout << "[!] No buff looter within 15min." << std::endl;
			fully_loot = true;
		}
		auto quality = crate.get_crate_quality();
		auto loot = loot_crate();

		set_completed();
		auto time_passed = util::get_elapsed<std::chrono::seconds>(start);
		core::StationResult result(this, true, time_passed, {});

		if (fully_loot) {
			send_success_embed(result, loot, quality, ++times_looted);
		}
		else {
			request_reroll(
				result, loot, quality, last_discovered_up + max_buff_wait_time);
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
	void LootCrateStation::set_fully_loot(bool fully_loot)
	{
		std::cout << "[+] Fully loot set to " << fully_loot << std::endl;
		this->fully_loot = fully_loot;
	}

	cv::Mat LootCrateStation::loot_crate()
	{
		asa::entities::local_player->access(crate);
		auto loot = asa::window::screenshot({ 1193, 227, 574, 200 });
		std::cout << "\t[-] Loot screenshot has been taken." << std::endl;

		// TO DO: Be smart about taking certain stuff and not taking other shit.
		if (!fully_loot) {
			crate.inventory->close();
			return loot;
		}

		do {
			crate.inventory->transfer_all();
		} while (!util::await([this]() { return !crate.inventory->is_open(); },
			std::chrono::seconds(6)));
		std::cout << "\t[-] The crate has been looted successfully."
				  << std::endl;
		return loot;
	}

	void LootCrateStation::teleport_to()
	{
		asa::entities::local_player->teleport_to(
			teleporter, can_default_teleport);

		if (!can_default_teleport) {
			asa::entities::local_player->turn_up(
				60, std::chrono::milliseconds(500));
		}
	}

	bool LootCrateStation::has_buff_wait_expired() const
	{
		return util::timedout(last_discovered_up, max_buff_wait_time);
	}


}
