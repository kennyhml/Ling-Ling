
#include "bots/paste/embeds.h"
#include "bots/paste/pastestation.h"
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

	std::array<llpp::bots::paste::PasteStation*, 10> stations;


	for (int i = 0; i < 10; i++) {

		auto station = new llpp::bots::paste::PasteStation(
			asa::structures::SimpleBed("PASTE0" + std::to_string(i + 1)));
		stations[i] = station;
	}

	while (true) {
		for (auto station : stations) {
			station->Complete();
		}

		Sleep(200000);
	}


	return 0;
}