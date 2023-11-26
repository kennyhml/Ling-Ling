
#include "bots/paste/pastestation.h"
#include "core/station.h"
#include "core/webhook.h"
#include <asapp/game/globals.h>
#include <asapp/game/settings.h>
#include <asapp/game/window.h>

#include <dpp/dpp.h>

int main()
{

	llpp::core::discord::InitWebhook(
		"https://discord.com/api/webhooks/1178195420002922586/"
		"O2vfiRTSxDMsWJm2phzYuiGX8M16_2_yzs3OiFelmF6sK3d17yYR53byu7fpKaHM-Y3S");

	asa::globals::gameBaseDirectory = std::filesystem::path(
		"F:\\SteamLibrary\\steamapps\\common\\ARK Survival Ascended");

	asa::window::GetHandle(60, true);
	asa::window::SetForeground();
	asa::settings::LoadGameUserSettings();
	asa::settings::LoadActionMappings();

	asa::resources::assetsDir = std::filesystem::path(
		"C:\\dev\\ASAPP\\ASAPP\\assets");
	asa::resources::Init();
	asa::items::Init();

	auto station = llpp::bots::paste::PasteStation(std::string("PASTE01"));

	return 0;
}