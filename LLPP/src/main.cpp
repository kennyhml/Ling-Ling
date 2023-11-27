
#include "bots/paste/embeds.h"
#include "bots/paste/pastestationmanager.h"
#include "core/webhook.h"
#include <asapp/game/globals.h>
#include <asapp/game/settings.h>
#include <asapp/game/window.h>

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{

	std::ifstream f("src/config.json");
	json data = json::parse(f);
	f.close();

	llpp::core::discord::InitWebhook(data["webhook"]);

	asa::globals::gameBaseDirectory = std::filesystem::path(
		std::string(data["gameBaseDirectory"]));

	asa::window::GetHandle(60, true);
	asa::window::SetForeground();
	asa::settings::LoadGameUserSettings();
	asa::settings::LoadActionMappings();

	asa::resources::assetsDir = std::filesystem::path(
		std::string(data["assetsDir"]));

	asa::items::itemdataPath = std::filesystem::path(
		std::string(data["itemData"]));


	asa::resources::Init();
	asa::items::Init();


	auto pasteManager = llpp::bots::paste::PasteStationManager("PASTE", 11);

	while (true) {
		pasteManager.CompleteReadyStations();
	}

	return 0;
}