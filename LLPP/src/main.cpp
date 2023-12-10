
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

using json = nlohmann::json;

int main()
{
	asa::Init(std::filesystem::path("src/config.json"));

	std::ifstream f("src/config.json");
	json data = json::parse(f);
	f.close();

	llpp::core::discord::Init(data["bot"]);

	asa::window::GetHandle(60, true);
	asa::window::SetForeground();

	using Quality = asa::structures::CaveLootCrate::Quality;

	auto suicideStation = llpp::bots::suicide::SuicideStation(
		"SUICIDE DEATH", "SUICIDE RESPAWN");

	auto swamp = llpp::bots::drops::CrateManager("SWAMP::",
		{ { Quality::RED, Quality::RED },
			{ Quality::YELLOW, Quality::YELLOW, Quality::ANY },
			{ Quality::BLUE } },
		std::chrono::minutes(5), &suicideStation);

	auto paste = llpp::bots::paste::PasteManager(
		"PASTE", 6, std::chrono::minutes(50));
	auto skylord = llpp::bots::drops::CrateManager("SKYLORD::",
		{
			{ Quality::YELLOW | Quality::RED, Quality::YELLOW | Quality::RED,
				Quality::YELLOW | Quality::RED },
		},
		std::chrono::minutes(5));

	while (true) {
		try {
			if (swamp.Run())
				continue;
			if (skylord.Run())
				continue;
			if (paste.Run())
				continue;


			std::cout << "No task ready...." << std::endl;
		}
		catch (asa::exceptions::ShooterGameError& e) {
			llpp::core::InformCrashDetected(e);
			llpp::core::Recover();
		}
		catch (const std::exception& e) {
			llpp::core::discord::InformFatalError(e, "???");
			break;
		}
	}
	return 0;
}