
#include "bots/drops/lootcratestation.h"
#include "bots/paste/pastestationmanager.h"
#include "core/webhook.h"
#include <asapp/game/globals.h>
#include <asapp/game/settings.h>
#include <asapp/game/window.h>
#include <asapp/init.h>
#include <asapp/structures/cavelootcrate.h>

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

	using CrateQuality = asa::structures::CaveLootCrate::Quality;

	auto crate = asa::structures::CaveLootCrate(CrateQuality::BLUE);

	auto station = llpp::bots::drops::LootCrateStation("SWAMP::DROP06", crate);

	auto actionwheel = asa::interfaces::ActionWheel();
	auto result = station.Complete();
	return 0;
}