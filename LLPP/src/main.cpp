
#include "bots/drops/cratemanager.h"
#include "bots/drops/lootcratestation.h"
#include "bots/paste/pastemanager.h"
#include "bots/suicide/suicidestation.h"
#include "core/discord.h"
#include "core/recovery.h"
#include <asapp/core/exceptions.h>
#include <asapp/entities/localplayer.h>

#include <asapp/core/init.h>
#include <asapp/entities/exceptions.h>
#include <asapp/interfaces/serverselect.h>
#include <dpp/dpp.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "bots/drops/embeds.h"
#include "core/data/database.h"
#include "core/data/managedvar.h"

using json = nlohmann::json;

int main()
{
	llpp::core::data::init("test.db");

	asa::core::init(std::filesystem::path("src/config.json"));

	std::ifstream f("src/config.json");
	json data = json::parse(f);
	f.close();

	llpp::core::discord::init(data["bot"]);

	asa::window::get_handle(60, true);
	asa::window::set_foreground();

	using Quality = asa::structures::CaveLootCrate::Quality;

	auto suicideStation = llpp::bots::suicide::SuicideStation(
		"SUICIDE DEATH", "SUICIDE RESPAWN");

	auto swamp = llpp::bots::drops::CrateManager("SWAMP",
		{ { Quality::RED, Quality::RED },
			{ Quality::YELLOW, Quality::YELLOW, Quality::ANY },
			{ Quality::BLUE } },
		std::chrono::minutes(5), &suicideStation);

	auto paste = llpp::bots::paste::PasteManager(
		"PASTE", 6, std::chrono::minutes(50));
	auto skylord = llpp::bots::drops::CrateManager("SKYLORD",
		{
			{ Quality::YELLOW | Quality::RED, Quality::YELLOW | Quality::RED,
				Quality::YELLOW | Quality::RED },
		},
		std::chrono::minutes(5));

	llpp::core::discord::bot->start(dpp::st_return);

	while (true) {
		try {
			if (swamp.run())
				continue;
			if (paste.run())
				continue;
			if (skylord.run())
				continue;

			std::cout << "No task ready...." << std::endl;
		}
		catch (asa::core::ShooterGameError& e) {
			llpp::core::inform_crash_detected(e);
			llpp::core::reconnect_to_server();
		}
		catch (const std::exception& e) {
			llpp::core::discord::inform_fatal_error(e, "???");
			break;
		}
	}
	return 0;
}