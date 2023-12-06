
#include "bots/drops/cratemanager.h"
#include "bots/drops/lootcratestation.h"
#include "bots/paste/pastestationmanager.h"
#include "bots/suicide/suicidestation.h"
#include "core/recovery.h"
#include "core/webhook.h"
#include <asapp/core/exceptions.h>
#include <asapp/entities/localplayer.h>

#include <asapp/core/init.h>

#include <asapp/entities/exceptions.h>
#include <asapp/interfaces/serverselect.h>

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{
	asa::Init(std::filesystem::path("src/config.json"));

	std::ifstream f("src/config.json");
	json data = json::parse(f);
	f.close();

	llpp::core::discord::InitWebhooks(data["webhook"], data["dropWebhook"]);

	asa::window::GetHandle(60, true);
	asa::window::SetForeground();

	using Quality = asa::structures::CaveLootCrate::Quality;

	auto suicideStation = llpp::bots::suicide::SuicideStation(
		"SUICIDE DEATH", "SUICIDE RESPAWN");

	auto st = llpp::bots::paste::PasteStation("meow");
	auto swamp = llpp::bots::drops::CrateManager("SWAMP::", 6,
		{ { Quality::RED, Quality::RED },
			{ Quality::YELLOW, Quality::YELLOW, Quality::ANY },
			{ Quality::BLUE } },
		std::chrono::minutes(10), &suicideStation);

	auto paste = llpp::bots::paste::PasteStationManager("PASTE", 6);
	auto skylord = llpp::bots::drops::CrateManager("SKYLORD::", 3,
		{
			{ Quality::YELLOW | Quality::RED, Quality::YELLOW | Quality::RED,
				Quality::YELLOW | Quality::RED },
		},
		std::chrono::minutes(15));

	paste.CompleteReadyStations();
	exit(1);


	while (true) {
		try {
			if (swamp.CompleteReadyStations())
				continue;
			if (skylord.CompleteReadyStations())
				continue;
			std::cout << "No task ready...." << std::endl;
		}
		catch (asa::exceptions::ShooterGameError& e) {
			llpp::core::InformCrashDetected(e);
			llpp::core::Recover();
		}

		catch (const std::exception& e) {
			llpp::core::discord::InformFatalError(e, "Paste");
			break;
		}
	}
	return 0;
}