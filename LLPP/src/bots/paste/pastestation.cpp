#include "pastestation.h"
#include "../../common/util.h"
#include "embeds.h"
#include <asapp/entities/exceptions.h>
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/exceptions.h>

namespace llpp::bots::paste
{
	PasteStation::PasteStation(std::string name, std::chrono::minutes interval)
		: BaseStation(name, interval), bed(asa::structures::SimpleBed(name)){};

	core::StationResult PasteStation::complete()
	{
		auto start = std::chrono::system_clock::now();

		asa::entities::local_player->fast_travel_to(bed);
		empty_all();
		int pasteObtained = deposit_paste();
		set_completed();

		auto duration = util::get_elapsed<std::chrono::seconds>(start);
		core::StationResult resultData(
			this, true, duration, { { "Achatina Paste", pasteObtained } });

		send_success_embed(resultData);
		return resultData;
	}

	bool PasteStation::empty(asa::entities::DinoEnt& achatina)
	{
		try {
			asa::entities::local_player->access(achatina);
		}
		catch (const asa::entities::EntityNotAccessed& e) {
			send_achatina_not_accessible(name, e.get_entity()->get_name());
			return true;
		}

		auto& slot = achatina.get_inventory()->slots[0];
		bool hasPaste = slot.has_item(asa::items::resources::achatina_paste);
		if (hasPaste) {
			achatina.get_inventory()->take_slot(0);
		}
		achatina.exit();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return hasPaste;
	}

	void PasteStation::empty_all()
	{
		std::cout << "[+] Emptying the achatinas..." << std::endl;
		empty(achatinas[4]);
		do {
			asa::entities::local_player->turn_left(45);
		} while (!empty(achatinas[3]));
		do {
			asa::entities::local_player->turn_right(90);
		} while (!empty(achatinas[5]));

		asa::entities::local_player->crouch();
		asa::entities::local_player->turn_down(
			12, std::chrono::milliseconds(300));

		empty(achatinas[2]);
		do {
			asa::entities::local_player->turn_left(45);
		} while (!empty(achatinas[1]));

		do {
			asa::entities::local_player->turn_left(45);
		} while (!empty(achatinas[0]));
	}

	int PasteStation::deposit_paste()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		asa::entities::local_player->turn_left(
			135, std::chrono::milliseconds(300));

		int amount = 0;
		asa::entities::local_player->deposit_into_dedi(
			asa::items::resources::achatina_paste, &amount);
		return amount;
	}
}
