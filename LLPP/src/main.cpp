
#include "bots/drops/lootcratestation.h"
#include "bots/paste/pastestationmanager.h"
#include "core/webhook.h"
#include <asapp/game/globals.h>
#include <asapp/game/settings.h>
#include <asapp/game/window.h>
#include <asapp/init.h>
#include <asapp/structures/cavelootcrate.h>

#include "bots/drops/cratemanager.h"
#include "bots/suicide/suicidestation.h"
#include <asapp/entities/localplayer.h>
#include <asapp/interfaces/actionwheel.h>
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

	auto paste = llpp::bots::paste::PasteStationManager("PASTE", 11);

	auto swamp = llpp::bots::drops::CrateManager("SWAMP::", 6,
		{ { Quality::RED, Quality::RED },
			{ Quality::YELLOW, Quality::YELLOW, Quality::ANY },
			{ Quality::BLUE } },
		std::chrono::minutes(10), &suicideStation);

	auto skylord = llpp::bots::drops::CrateManager("SKYLORD::", 3,
		{
			{ Quality::YELLOW | Quality::RED, Quality::YELLOW | Quality::RED,
				Quality::YELLOW | Quality::RED },
		},
		std::chrono::minutes(15));

	while (true) {
		if (swamp.CompleteReadyStations())
			continue;
		if (skylord.CompleteReadyStations())
			continue;
		std::cout << "No task ready...." << std::endl;
	}
	return 0;
}