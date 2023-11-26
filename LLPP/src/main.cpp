
#include "bots/paste/pastestation.h"
#include "core/station.h"
#include "core/webhook.h"
#include <asapp/game/globals.h>
#include <asapp/game/settings.h>
#include <asapp/game/window.h>

#include <dpp/dpp.h>

int main()
{

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

	auto station = llpp::bots::paste::PasteStation(std::string("PASTE05"));


	station.Complete();
	return 0;
}